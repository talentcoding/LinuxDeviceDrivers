//----------Simple Linux device driver File Operations 


#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>

dev_t dev = 0;
static struct class *mydev_class;
static struct cdev my_cdev;

// Function Prototypes

static int      __init my_driver_init(void);
static void     __exit my_driver_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);

static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = my_read,
    .write      = my_write,
    .open       = my_open,
    .release    = my_release,
};

static int my_open(struct inode *inode, struct file *file)
{
        pr_info("my_open driver Open Function Called...!!!\n");
        return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
        pr_info("my_release driver Release Function Called...!!!\n");
        return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("my_read driver Read Function Called...!!!\n");
        return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("my_write driver Write Function Called...!!!\n");
        return len;
}

// Module Init function
static int __init my_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "My_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
	
	/*Creating cdev structure*/
        cdev_init(&my_cdev,&fops);
        /*Adding character device to the system*/
        if((cdev_add(&my_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }

	/*Creating struct class*/
        if(IS_ERR(mydev_class = class_create("Talent_class"))){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
        /*Creating device*/
        if(IS_ERR(device_create(mydev_class,NULL,dev,NULL,"Talent_device"))){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }

        pr_info("Device Driver Insert...Done!!!\n");
      return 0;
	
      r_device:
        class_destroy(mydev_class);
      r_class:
        unregister_chrdev_region(dev,1);
        return -1;

}

//  Module exit function
static void __exit my_driver_exit(void)
{
	device_destroy(mydev_class,dev);
        class_destroy(mydev_class);
        unregister_chrdev_region(dev, 1);
	cdev_del(&my_cdev);
        pr_info("Device Driver Remove...Done!!!\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("santhosh");
MODULE_DESCRIPTION("Basic linux driver File Operations program ");
MODULE_VERSION("1.0");
