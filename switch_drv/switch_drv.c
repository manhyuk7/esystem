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
#define MAJOR_NUMBER   102
#define DEVICE_NAME      "switch_dd"
#define GPIO_LED1      18 /* LED1 gpio number */
#define GPIO_LED2      23 /* LED2 gpio number */
#define GPIO_SW         25 /* Switch gpio number */

static int switch_irq;     /* switch gpio's IRQ number */
static int irq_count = 0;  /* interrupt count */
static char irq_info[2];   /* interrupt info for returnning to app */

/* gpio ISR : interrupt service routine */
static irqreturn_t isr_func(int irq, void *dev){

    static int gpio_value;
	irq_count++; /* interrupt count ++ */

    gpio_value = gpio_get_value(GPIO_LED1); /* GPIO_LED1 value */

    printk("Switch driver: isr total count = %d\n", irq_count);
    printk("Switch driver: isr irq number  = %d\n", irq);
    printk("Switch driver: isr parameter   = %s\n", dev);
    printk("Switch driver: gpio_get_value  = %d\n", gpio_value);
 
    gpio_set_value(GPIO_LED1, !gpio_value);
    gpio_set_value(GPIO_LED2, !gpio_value);

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

/* $sudo mknod /dev/button_dd 255 0 */
/* $sudo chmod 666 /dev/button_dd */
static int switch_init(void) {
    int err, i;

    printk(KERN_INFO "Switch driver: driver initialized!\n");

    if((register_chrdev(MAJOR_NUMBER, DEVICE_NAME, &gpio_fops)) < 0) {
	printk(KERN_INFO "can't be register\n");
	return MAJOR_NUMBER;
    }
	
    /* gpio register */
    gpio_request(GPIO_LED1, "LED");      /* GPIO allocation */
    gpio_direction_output(GPIO_LED1, 0); /* GPIO set as output */
    gpio_request(GPIO_LED2, "LED");      /* GPIO GPIO allocation */
    gpio_direction_output(GPIO_LED2, 0); /* GPIO set as output */

    for(i = 0; i < 5; i++) {
        gpio_set_value(GPIO_LED1, 1);
        gpio_set_value(GPIO_LED2, 1);
        mdelay(500);
	gpio_set_value(GPIO_LED1, 0);
	gpio_set_value(GPIO_LED2, 0);
        mdelay(500);
    }
	
    /* register interrupt handler */
    gpio_request(GPIO_SW, "SWITCH"); /* GPIO allocation */
    switch_irq = gpio_to_irq(GPIO_SW); /* return irq number. GPIO irq happens when gpio input change */
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

    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME); /* /dev/button_dd unregister */
	
    /* Interrupt ?´ì œ */
    free_irq(switch_irq, NULL);
    gpio_free(GPIO_SW);
    gpio_free(GPIO_LED1);
    gpio_free(GPIO_LED2);

    printk(KERN_INFO "Switch driver: released!\n");
}

module_init(switch_init);
module_exit(switch_exit);
MODULE_LICENSE("GPL v2");