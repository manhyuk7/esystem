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

#define DEVICE_NAME "led_dd"  /* mknod /dev/led_dd c 241 0 */

#define LED_OFF 0
#define LED_ON 1
#define GPIO_LED1      18 /* LED1 gpio number */
#define GPIO_LED2      23 /* LED2 gpio number */

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

    
    for(i=0; i<len; i++){
    	copy_from_user(&state, &buf[i], 1); /* read the led status request */

	printk("state = %c, buf[i] = %c\n", state, buf[i]);

        
        if (state == '0') {
           gpio_set_value(GPIO_LED1, 0);
           gpio_set_value(GPIO_LED2, 0);
           mdelay(1000);
           gpio_set_value(GPIO_LED1, 1);
           gpio_set_value(GPIO_LED2, 1);
           mdelay(1000);
           gpio_set_value(GPIO_LED1, 0);
           gpio_set_value(GPIO_LED2, 0);

	 printk("[led_dd] led_write OFF: GPIO %d\n", GPIO_LED2);
	 printk("[led_dd] led_write OFF: GPIO %d\n", GPIO_LED2);

	}
       else {
           gpio_set_value(GPIO_LED1, 1);
           gpio_set_value(GPIO_LED2, 1);
           mdelay(1000);
           gpio_set_value(GPIO_LED1, 0);
           gpio_set_value(GPIO_LED2, 0);
           mdelay(1000);
           gpio_set_value(GPIO_LED1, 1);
           gpio_set_value(GPIO_LED2, 1);

	 printk("[led_dd] led_write ON: GPIO %d\n", GPIO_LED2);
	 printk("[led_dd] led_write ON: GPIO %d\n", GPIO_LED2);
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

    register_chrdev(DRV_MAJOR, DEVICE_NAME, &fops);

    /* gpio register */
    gpio_request(GPIO_LED1, "LED");         /* GPIO allocation */
    gpio_direction_output(GPIO_LED1, 0);   /* GPIO set as output */
    gpio_request(GPIO_LED2, "LED");         /* GPIO GPIO allocation */
    gpio_direction_output(GPIO_LED2, 0);   /* GPIO set as output */

    for(i = 0; i < 5; i++) {
        gpio_set_value(GPIO_LED1, 1);
        gpio_set_value(GPIO_LED2, 1);
        mdelay(1000);
	gpio_set_value(GPIO_LED1, 0);
	gpio_set_value(GPIO_LED2, 0);
        mdelay(1000);
    }

    printk(KERN_INFO "LED: /dev/led_dd, major No = %d\n", DRV_MAJOR);
    return 0;
}

static void led_exit(void){
    unregister_chrdev(DRV_MAJOR, DEVICE_NAME);

    /* Interrupt release */
    gpio_free(GPIO_LED1);
    gpio_free(GPIO_LED2);

    printk(KERN_INFO "[led_dd] exit: major No = %d\n", DRV_MAJOR);
}

module_init(led_init); /* led_init */
module_exit(led_exit); /* led_exit */