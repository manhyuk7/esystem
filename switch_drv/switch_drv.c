#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/major.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/version.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/timer.h> 
#include <linux/gpio.h> /* gpio functions */
#include <linux/interrupt.h>

/* BCM2711 CPU */
#define DEVICE_NAME "switch_dd"  /* mknod /dev/led_dd c 102 0 */
#define MAJOR_NUMBER   102  /* device number : major */

#define GPIO_BASE 0xFE200000    /* 라즈베리파이 4의 GPIO 베이스 주소 */

#define GPIO_SET_OFFSET_18 0x1C    /* LED 18 : SET Offset */
#define GPIO_CLR_OFFSET_18 0x28    /* LED 18:  Clear Offset */
#define GPIO_SEL_OFFSET_18 0x04    /* LED 18:  Selection Offset */

#define GPIO_SET_OFFSET_23 0x1C    /* LED 23 : SET Offset */
#define GPIO_CLR_OFFSET_23 0x28    /* LED 23:  Clear Offset */
#define GPIO_SEL_OFFSET_23 0x08    /* LED 23:  Selection Offset */

#define GPIO_SEL_OFFSET_25 0x08    /* LED 25 : SET Offset */

#define GPIO_SW         25 /* Switch gpio number */

volatile void __iomem *gpio_base;
unsigned int sel_val;

static int switch_irq;     /* switch gpio's IRQ number */
static int irq_count = 0;  /* interrupt count */
static char irq_info[2];   /* interrupt info for returnning to app */

/* gpio ISR : interrupt service routine */
static irqreturn_t isr_func(int irq, void *dev){

    irq_count++; /* interrupt count ++ */

    printk("Switch driver: isr total count = %d\n", irq_count);
    printk("Switch driver: isr irq number  = %d\n", irq);
    printk("Switch driver: isr parameterr  = %s\n", dev);

   if(irq_count % 2 == 0) {
    // GPIO 18/23: LED 끄기
        writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);
   }
   else {
    // GPIO18/23: LED 켜기
        writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
   }   

        return IRQ_HANDLED;
}

/* device driver open */
static int switch_open(struct inode *inode, struct file *filp){
    return 0;
}

/* device driver close */
static int switch_release(struct inode *inode, struct file *filp) {
   return 0;
}

/* Device driver write */
static ssize_t switch_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
   return 0;
}

/* Device driver read */
static ssize_t switch_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
   irq_info[0] = switch_irq;
   irq_info[1] = irq_count;

   copy_to_user(buf, &irq_info, sizeof(irq_info)); /* kernel -> user : interrupt information */
   
   return count;
}

static long switch_ioctl(struct file *flip, unsigned int cmd, unsigned long arg){
   return 0;
}

/* gpio file operations */
static struct file_operations gpio_fops = {
   .owner = THIS_MODULE,
   .read = switch_read,
   .write = switch_write,
   .open = switch_open,
   .release = switch_release,
   .unlocked_ioctl = switch_ioctl
};

/* $sudo mknod /dev/switch_dd 102 0 */
/* $sudo chmod 666 /dev/switch_dd */

static int switch_init(void) {
    int err, i;

    printk(KERN_INFO "Switch driver: driver initialized!\n");

    if((register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &gpio_fops)) < 0) {
	printk(KERN_INFO "can't be register\n");
	return MAJOR_NUMBER;
    }
	
    /* gpio memory mapping */
    gpio_base = ioremap(GPIO_BASE, 0x100);
    if (!gpio_base) {
        pr_err("Failed to map GPIO memory\n");
        return -ENOMEM;
    }

    /* gpio register */
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

  // GPIO 핀 설정 (예: GPIO 25을 입력으로 설정)
    sel_val = readl(gpio_base + GPIO_SEL_OFFSET_25);
    sel_val &= ~(7 << 15);            // GPIO 25 위치 초기화
    sel_val |= (0 << 15);                // GPIO 25 을 출력으로 설정
    writel(sel_val, gpio_base + GPIO_SEL_OFFSET_25);

    for(i = 0; i < 5; i++) {
    // GPIO18/23: LED 켜기
        writel(1 << 18, gpio_base + GPIO_SET_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_SET_OFFSET_23);
        mdelay(1000); 

    // GPIO 18/23: LED 끄기
        writel(1 << 18, gpio_base + GPIO_CLR_OFFSET_18);
        writel(1 << 23, gpio_base + GPIO_CLR_OFFSET_23);
        mdelay(1000);
    }
	
    /* register interrupt handler */
      switch_irq = gpio_to_irq(GPIO_SW); /* return irq number. GPIO irq happens when gpio input change */
                                                     /* 라즈베리파이4는 GPIO 컨트롤러와 IRQ 컨트롤러가 매핑되어 있음 */
                                                     /*  /proc/interrupt: 57:          3          0          0          0  pinctrl-bcm2835  25 Edge */
    /* register interrupt service*/ 
    err = request_irq(switch_irq, isr_func, IRQF_TRIGGER_RISING, "switch", "GOOD");

    printk(KERN_INFO "Switch driver: switch_irq = %d\n", switch_irq);
    if (err < 0) {
        printk(KERN_ERR "ERROR : request_irq\n");
        return -1;
    }

    return 0;
}

static void switch_exit(void){

    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME); /* /dev/switch_dd unregister */

    /* Interrupt release */
    free_irq(switch_irq, "GOOD");

   // 메모리 매핑 해제
    if (gpio_base)
        iounmap(gpio_base);

    printk(KERN_INFO "Switch driver: released!\n");
}

module_init(switch_init);
module_exit(switch_exit);
MODULE_LICENSE("GPL v2");
