#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/device.h>

dev_t dev = 0;
static struct class *My_class;

static int __init dynamic_device_init(void)
{
    // Allocating Major number
    if (alloc_chrdev_region(&dev, 0, 1, "My_dev") < 0) {
        pr_err("Cannot allocate major number for device\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n", MAJOR(dev), MINOR(dev));

    // Creating struct class
    My_class = class_create("My_class");
    if (IS_ERR(My_class)) {
        pr_err("Cannot create the struct class for device\n");
        goto r_class; // Clean up when class creation fails
    }

    // Creating device
    if (IS_ERR(device_create(My_class, NULL, dev, NULL, "My_device"))) {
        pr_err("Cannot create the Device\n");
        goto r_device; // Clean up when device creation fails
    }

    pr_info("Kernel Module Inserted Successfully...\n");
    return 0;

r_device:
    class_destroy(My_class); // Clean up the class if device creation fails

r_class:
    unregister_chrdev_region(dev, 1); // Clean up the allocated major number if class creation fails

    return -1;
}

static void __exit dynamic_device_exit(void)
{
    // Clean up and unregister the device and class
    device_destroy(My_class, dev); // Destroy the device
    class_destroy(My_class); // Destroy the class
    unregister_chrdev_region(dev, 1); // Unregister the major number

    pr_info("Kernel Module Removed Successfully...\n");
}

module_init(dynamic_device_init);
module_exit(dynamic_device_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION(" Dynamic Device file allocation example");
MODULE_VERSION("1.0");
