//  Basic Char driver 

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/err.h>
 

#define mem_size        1024           
 
dev_t dev = 0;
static struct class *my_dev_class;
static struct cdev my_cdev;
uint8_t *kernel_buffer;


// Function Prototypes

static int      __init my_driver_init(void);
static void     __exit my_driver_exit(void);
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);



// File Operations structure

static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_read,
        .write          = my_write,
        .open           = my_open,
        .release        = my_release,
};
 

// This function will be called when we open the Device file

static int my_open(struct inode *inode, struct file *file)
{
        pr_info("------my_open function Opened ----!!\n");
        return 0;
}


// This function will be called when we close the Device file

static int my_release(struct inode *inode, struct file *file)
{
        pr_info("------ my_release function closed -----!\n");
        return 0;
}


// This function will be called when we read the Device file

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        //Copy the data from the kernel space to the user-space
        if( copy_to_user(buf, kernel_buffer, mem_size) )
        {
                pr_err("Data Read : Err!\n");
        }
        pr_info(" my_read function Data Read : Done!\n");
        return mem_size;
}


// This function will be called when we write the Device file

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        //Copy the data to kernel space from the user-space
        if( copy_from_user(kernel_buffer, buf, len) )
        {
                pr_err("Data Write : Err!\n");
        }
        pr_info("--my_write function Data Write : Done!\n");
        return len;
}


// Module Init function

static int __init my_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_Dev")) <0){
                pr_info("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&my_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&my_cdev,dev,1)) < 0){
            pr_info("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if(IS_ERR(my_dev_class = class_create("my_class"))){
            pr_info("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if(IS_ERR(device_create(my_dev_class,NULL,dev,NULL,"my_device"))){
            pr_info("Cannot create the Device 1\n");
            goto r_device;
        }
        
        /*Creating Physical memory*/
        if((kernel_buffer = kmalloc(mem_size , GFP_KERNEL)) == 0){
            pr_info("Cannot allocate memory in kernel\n");
            goto r_device;
        }
        
        strcpy(kernel_buffer, "Santhosh");
        
        pr_info("----Device Driver Inserted...Done-----\n");
        return 0;
 
r_device:
        class_destroy(my_dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}


// Module exit function

static void __exit my_driver_exit(void)
{
  kfree(kernel_buffer);
        device_destroy(my_dev_class,dev);
        class_destroy(my_dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("----Device Driver Remove...Done----\n");
}
 
module_init(my_driver_init);
module_exit(my_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION("Simple Char Linux device driver");
MODULE_VERSION("1.0");
