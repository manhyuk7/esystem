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
#include <linux/irq.h>

/* BCM2711 CPU */
#define MAJOR_NUMBER   104
#define DEVICE_NAME      "button_dd"
#define GPIO_LED1      18 /* LED1 gpio number */
#define GPIO_LED2      23 /* LED2 gpio number */
#define GPIO_SW         25 /* Switch gpio number */

/* switch gpio's IRQ number */
static int switch_irq;

static void isr_func(struct tasklet_struct *unused);
DECLARE_TASKLET(switch_tasklet, isr_func); 

static irqreturn_t isr_tasklet(int irq, void *data){

    static int num = 1;

    printk("Button driver: isr num = %d\n", num++);
    printk("Button driver: isr irq = %d\n", irq);

    tasklet_disable(&switch_tasklet);
    tasklet_schedule(&switch_tasklet);
    tasklet_enable(&switch_tasklet);

    return IRQ_HANDLED;
}

/* gpio ISR : interrupt service routine */
static void isr_func(struct tasklet_struct *unused) {

    static int gpio_value;

    gpio_value = gpio_get_value(GPIO_LED1); /* GPIO_LED1 value */
    printk("Button driver: gpio_get_value = %d\n", gpio_value);
    printk("Button driver: tasklet was scheduled\n");
 
    gpio_set_value(GPIO_LED1, !gpio_value);
    gpio_set_value(GPIO_LED2, !gpio_value);
}

/* device driver open */
static int button_open(struct inode *inode, struct file *filp){
    return 0;
}

/* device driver close */
static int button_release(struct inode *inode, struct file *filp) {
   return 0;
}

/* Device driver write */
static ssize_t button_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
   return 0;
}

/* Device driver read */
static ssize_t button_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {
   return 0;
}

static long button_ioctl(struct file *flip, unsigned int cmd, unsigned long arg){
   return 0;
}

/* gpio file operations */
static struct file_operations gpio_fops = {
   .owner = THIS_MODULE,
   .read = button_read,
   .write = button_write,
   .open = button_open,
   .release = button_release,
   .unlocked_ioctl = button_ioctl
};

/* $sudo mknod /dev/switch_dd 104 0 */
static int switch_init(void) {
    dev_t devno;
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

    err = request_irq(switch_irq, isr_tasklet, IRQF_TRIGGER_RISING, "switch", "GOOD");

    tasklet_schedule(&switch_tasklet);

    printk(KERN_INFO "Switch driver: switch_irq = %d\n", switch_irq);
    if (err < 0) {
        printk(KERN_ERR "ERROR : request_irq\n");
        return -1;
    }

    return 0;
}

static void switch_exit(void){

    unregister_chrdev(MAJOR_NUMBER, DEVICE_NAME); /* /dev/switch_dd unregister */
	
    /* Interrupt Release */
    free_irq(switch_irq, "GOOD");
    gpio_free(GPIO_SW);
    gpio_free(GPIO_LED1);
    gpio_free(GPIO_LED2);
    tasklet_kill(&switch_tasklet);

    printk(KERN_INFO "Switch driver: released!\n");
}

module_init(switch_init);
module_exit(switch_exit);
MODULE_LICENSE("GPL v2");