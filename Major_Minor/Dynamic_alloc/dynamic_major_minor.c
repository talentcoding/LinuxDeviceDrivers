
// Dynamic allocation of Major and Minor numbers

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>

dev_t dev = 0;

static int __init Major_Minor_init(void)
{
    if((alloc_chrdev_region(&dev, 0, 1, "Santhosh_Dev")) <0){
                pr_info(KERN_INFO "Cannot allocate major number for device 1 \n");
                return -1;
        }  

    pr_info(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
    pr_info(KERN_INFO "-----Kernel Module Inserted Successfully------\n");
    return 0;
}


static void __exit Major_Minor_exit(void)
{
    unregister_chrdev_region(dev, 1);     // un-registering call of major and minor

    pr_info(KERN_INFO "-----Kernel Module Removed Successfully-----\n");
}
 
module_init(Major_Minor_init);
module_exit(Major_Minor_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION("Dynamically allocating the Major and Minor number");
