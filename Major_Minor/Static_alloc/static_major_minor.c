
// static allocation of Major and Minor numbers

#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include <linux/fs.h>

//creating major and minor number it will appear major 260 and minor 0 in /proc/devices
dev_t dev = MKDEV(260, 0);

static int __init Major_Minor_init(void)
{
    register_chrdev_region(dev, 1, "Santhosh_Dev");   // statically registering call of major and minor  

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
MODULE_DESCRIPTION("Static allocating the Major and Minor number");
