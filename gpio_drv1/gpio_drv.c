#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <linux/gpio.h> /* gpio functions */

MODULE_LICENSE("Dual BSD/GPL");

#define DEVICE_NAME "led_dd"  /* mknod /dev/led_dd c 101 0 */

#define GPIO_BASE 0xFE200000    /* 라즈베리파이 4의 GPIO 베이스 주소 */

#define GPIO_SET_OFFSET_18 0x1C    /* LED 18 : SET Offset */
#define GPIO_CLR_OFFSET_18 0x28    /* LED 18:  Clear Offset */
#define GPIO_SEL_OFFSET_18 0x04    /* LED 18:  Selection Offset */

#define GPIO_SET_OFFSET_23 0x1C    /* LED 23 : SET Offset */
#define GPIO_CLR_OFFSET_23 0x28    /* LED 23:  Clear Offset */
#define GPIO_SEL_OFFSET_23 0x08    /* LED 23:  Selection Offset */

volatile void __iomem *gpio_base;
unsigned int sel_val;

#define DRV_MAJOR 101    /* device number : major */

/* led_open : all led off */
static int led_open(struct inode *inode, struct file *flip){
    printk("[led_dd] led_open\n");
    return 0;
}

static ssize_t led_read(struct file *file, char *buf, size_t count, loff_t *ppoa)
{
    printk("[led_dd] led_open\n");
    return 0;
}

static long led_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    printk(KERN_INFO "led_ioctl is invoked\n");
    return 0;
}

/* led close */
static int led_release(struct inode *inode, struct file *flip){
    printk("[led_dd] led_release\n");
    return 0;
}

/* led_write: LED on/off function */
static ssize_t led_write(struct file *file, const char *buf, size_t len, loff_t *ppoa){
    int i, k; 
    char state;
    
    for(i=0; i < len; i++){
    	k = copy_from_user(&state, &buf[i], 1); /* read the led status request */

	printk("state = %c, buf[i] = %c\n", state, buf[i]);

        
        if (state == '0') {

    // GPIO 18/23: LED 끄기
         writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
         writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);
         mdelay(1000);

   // GPIO18/23: LED 켜기
          writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
          writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
          mdelay(1000);

   // GPIO 18/23: LED 끄기
         writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
         writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);
       }
       else {
    // GPIO18/23: LED 켜기
          writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
          writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
          mdelay(1000);

     // GPIO 18/23: LED 끄기
         writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
         writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);
         mdelay(1000);

    // GPIO18/23: LED 켜기
          writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
          writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
     }
}
    
    printk("[led_dd] led_write\n");
    return len;
}

static struct file_operations fops={
    .owner = THIS_MODULE,
    .open = led_open,
    .read = led_read,
    .write = led_write,
    .compat_ioctl = led_ioctl,
    .release = led_release,   
};

static int led_init(void){
    int i;

   /* sudo mknod /dev/ded_dd c 101 0 */
    register_chrdev(DRV_MAJOR, DEVICE_NAME, &fops);

    /* gpio memory mapping */
    gpio_base = ioremap(GPIO_BASE, 0x100);
    if (!gpio_base) {
        pr_err("Failed to map GPIO memory\n");
        return -ENOMEM;
    }

    // GPIO 핀 설정 (예: GPIO 18을 출력으로 설정)
    sel_val = readl(gpio_base + GPIO_SEL_OFFSET_18);
    sel_val &= ~(7 << 24);            // GPIO 18 위치 초기화
    sel_val |= (1 << 24);                // GPIO 18을 출력으로 설정
    writel(sel_val, gpio_base + GPIO_SEL_OFFSET_18);

  // GPIO 핀 설정 (예: GPIO 23을 출력으로 설정)
    sel_val = readl(gpio_base + GPIO_SEL_OFFSET_23);
    sel_val &= ~(7 << 9);            // GPIO 23 위치 초기화
    sel_val |= (1 << 9);                // GPIO 23 을 출력으로 설정
   writel(sel_val, gpio_base + GPIO_SEL_OFFSET_23);

   printk(KERN_INFO "LED: sel_val %x\n", sel_val);

    for(i = 0; i < 5; i++) {

    // GPIO18/23: LED 켜기
        writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);

        mdelay(1000);     // 1초 대기

    // GPIO 18/23: LED 끄기
        writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);

        mdelay(1000);
    }

    printk(KERN_INFO "LED: /dev/led_dd, major No = %d\n", DRV_MAJOR);
    return 0;
}

static void led_exit(void){
    unregister_chrdev(DRV_MAJOR, DEVICE_NAME);

// 메모리 매핑 해제
    if (gpio_base)
        iounmap(gpio_base);

    printk(KERN_INFO "[led_dd] exit: major No = %d\n", DRV_MAJOR);
}

module_init(led_init); /* led_init */
module_exit(led_exit); /* led_exit */