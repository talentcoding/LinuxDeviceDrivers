
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include<linux/proc_fs.h>
#include <linux/err.h>

 
#define LINUX_KERNEL_VERSION  510
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
 
int32_t value = 0;
char my_array[20]=" universe \n";
static int len = 1;
 
 
dev_t dev = 0;
static struct class *my_dev_class;
static struct cdev my_cdev;
static struct proc_dir_entry *parent;

/*
** Function Prototypes
*/
static int      __init my_driver_init(void);
static void     __exit my_driver_exit(void);

/*************** Driver Functions **********************/
static int      my_open(struct inode *inode, struct file *file);
static int      my_release(struct inode *inode, struct file *file);
static ssize_t  my_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  my_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long     my_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
 
/***************** Procfs Functions *******************/
static int      my_open_proc(struct inode *inode, struct file *file);
static int      my_release_proc(struct inode *inode, struct file *file);
static ssize_t  my_read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset);
static ssize_t  my_write_proc(struct file *filp, const char *buff, size_t len, loff_t * off);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = my_read,
        .write          = my_write,
        .open           = my_open,
        .unlocked_ioctl = my_ioctl,
        .release        = my_release,
};


// procfs operation sturcture
static struct proc_ops proc_fops = {
        .proc_open = my_open_proc,
        .proc_read = my_read_proc,
        .proc_write = my_write_proc,
        .proc_release = my_release_proc
};


static int my_open_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file opend.....\t");
    return 0;
}

static int my_release_proc(struct inode *inode, struct file *file)
{
    pr_info("proc file released.....\n");
    return 0;
}

static ssize_t my_read_proc(struct file *filp, char __user *buffer, size_t length,loff_t * offset)
{
    pr_info("proc file read.....\n");
    if(len)
    {
        len=0;
    }
    else
    {
        len=1;
        return 0;
    }
    
    if( copy_to_user(buffer,my_array,20) )
    {
        pr_err("Data Send : Err!\n");
    }
 
    return length;;
}

static ssize_t my_write_proc(struct file *filp, const char *buff, size_t len, loff_t * off)
{
    pr_info("proc file wrote.....\n");
    
    if( copy_from_user(my_array,buff,len) )
    {
        pr_err("Data Write : Err!\n");
    }
    
    return len;
}

/*
** This function will be called when we open the Device file
*/
static int my_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/
static int my_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
        pr_info("Read function\n");
        return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
        pr_info("Write Function\n");
        return len;
}

static long my_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
         switch(cmd) {
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}
 

static int __init my_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "my_device")) <0){
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
        
        /*Create proc directory. It will create a directory under "/proc" */
        parent = proc_mkdir("My_Proc",NULL);
        
        if( parent == NULL )
        {
            pr_info("Error creating proc entry");
            goto r_device;
        }
        
        /*Creating Proc entry under "/proc/My_proc/" */
        proc_create("My_proc", 0666, parent, &proc_fops);
 
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(my_dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
 
static void __exit my_driver_exit(void)
{
        /* Removes single proc entry */
                
        // remove complete 
        proc_remove(parent);
        
        device_destroy(my_dev_class,dev);
        class_destroy(my_dev_class);
        cdev_del(&my_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(my_driver_init);
module_exit(my_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("santhosh");
MODULE_DESCRIPTION("procfs driver program");
MODULE_VERSION("1.0");
