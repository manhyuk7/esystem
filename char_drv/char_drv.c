#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");

#define DEVICE_NAME "char_dd"         /* mknod /dev/char_dd c 240 0 */
#define DRV_MAX_LENGTH 4096         /* device read/write length */
#define MIN(a,b) ((a) < (b) ? (a) : (b))  /* choose minimum */
#define DRV_MAJOR 240                   /* device number : major */
static char *drv_data;                        /* kernel memory buffer */
static int drv_read_offset, drv_write_offset; 

static int drv_open(struct inode *inode, struct file *file)
{
    if(MAJOR(inode->i_rdev) != DRV_MAJOR)  /* check driver number */
	return -1;
    return 0;
}

static int drv_release(struct inode *inode, struct file *file)
{
    if(MAJOR(inode->i_rdev) != DRV_MAJOR) /* check driver number */
        return -1;
    return 0;
}

static long drv_ioctl(struct file *file, unsigned int cmd, unsigned long arg) 
{
    printk(KERN_INFO "drv_ioctl is invoked\n");
    return 0;
}

static ssize_t drv_read(struct file *file, char *buf, size_t count, loff_t *ppoa)
{
    if((drv_write_offset - drv_read_offset) <= 0)
	return 0;

    count = MIN((drv_write_offset - drv_read_offset), count);
    copy_to_user(buf, drv_data + drv_read_offset, count); /* kernel -> user */

    drv_read_offset += count;
    return count;
}

static ssize_t drv_write(struct file *file, const char *buf, size_t count, loff_t *ppoa)
{

    if(count + drv_write_offset >= DRV_MAX_LENGTH) {
        return 0; /* driver space in too small */
    }

    copy_from_user(drv_data + drv_write_offset, buf, count); /* user -> kernel */

    drv_write_offset += count;
    return count;
}
	
struct file_operations drv_fops = {
    .owner = THIS_MODULE,
    .read = drv_read,
    .write = drv_write,
    .compat_ioctl = drv_ioctl,
    .open = drv_open,
    .release = drv_release,
};		

static int drv_init(void)
{
   register_chrdev(DRV_MAJOR, DEVICE_NAME, &drv_fops);  /* DEVICE_NAME: /dev/char_dd */
    printk(KERN_INFO "major No = %d\n", DRV_MAJOR);

    drv_data = (char *)kmalloc(DRV_MAX_LENGTH * sizeof(char), GFP_KERNEL);
    drv_read_offset = drv_write_offset = 0;

    return 0;
}

void drv_exit(void)
{
    kfree(drv_data);
    unregister_chrdev(DRV_MAJOR, DEVICE_NAME);

    printk(KERN_INFO "driver exit successful: major No = %d\n", DRV_MAJOR);
}

module_init(drv_init);
module_exit(drv_exit);