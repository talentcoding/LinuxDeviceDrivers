#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

#define DEVICE_NAME "my_device_sysfs"
#define CLASS_NAME  "my_class_sysfs"

static dev_t dev;
static struct cdev my_cdev;
static struct class *my_dev_class;
static struct device *my_device;
static struct kobject *kobj_ref;

static int my_value;

/* ---------- File operations ---------- */

static int my_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf,
                       size_t len, loff_t *off)
{
    return 0;
}

static ssize_t my_write(struct file *filp, const char __user *buf,
                        size_t len, loff_t *off)
{
    return len;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .read    = my_read,
    .write   = my_write,
};

/* ---------- Sysfs ---------- */

static ssize_t my_value_show(struct kobject *kobj,
                             struct kobj_attribute *attr,
                             char *buf)
{
    return sprintf(buf, "%d\n", my_value);
}

static ssize_t my_value_store(struct kobject *kobj,
                              struct kobj_attribute *attr,
                              const char *buf, size_t count)
{
    int ret;

    ret = kstrtoint(buf, 10, &my_value);
    if (ret)
        return ret;

    return count;
}

static struct kobj_attribute my_attr =
    __ATTR(my_value, 0664, my_value_show, my_value_store);

/* ---------- Init & Exit ---------- */

static int __init my_driver_init(void)
{
    int ret;

    /* Allocate major/minor */
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret) {
        pr_err("Failed to allocate char device\n");
        return ret;
    }

    /* Init and add cdev */
    cdev_init(&my_cdev, &fops);
    ret = cdev_add(&my_cdev, dev, 1);
    if (ret) {
        pr_err("Failed to add cdev\n");
        goto err_unregister;
    }

    /* Create class */
    my_dev_class = class_create(CLASS_NAME);
    if (IS_ERR(my_dev_class)) {
        pr_err("Failed to create class\n");
        ret = PTR_ERR(my_dev_class);
        goto err_cdev;
    }

    /* Create device */
    my_device = device_create(my_dev_class, NULL, dev, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        pr_err("Failed to create device\n");
        ret = PTR_ERR(my_device);
        goto err_class;
    }

    /* Create kobject under /sys/kernel/ */
    kobj_ref = kobject_create_and_add("my_sysfs", kernel_kobj);
    if (!kobj_ref) {
        pr_err("Failed to create kobject\n");
        ret = -ENOMEM;
        goto err_device;
    }

    /* Create sysfs file */
    ret = sysfs_create_file(kobj_ref, &my_attr.attr);
    if (ret) {
        pr_err("Failed to create sysfs file\n");
        goto err_kobj;
    }

    pr_info("Sysfs char driver loaded successfully\n");
    return 0;

err_kobj:
    kobject_put(kobj_ref);
err_device:
    device_destroy(my_dev_class, dev);
err_class:
    class_destroy(my_dev_class);
err_cdev:
    cdev_del(&my_cdev);
err_unregister:
    unregister_chrdev_region(dev, 1);

    return ret;
}

static void __exit my_driver_exit(void)
{
    sysfs_remove_file(kobj_ref, &my_attr.attr);
    kobject_put(kobj_ref);

    device_destroy(my_dev_class, dev);
    class_destroy(my_dev_class);

    cdev_del(&my_cdev);
    unregister_chrdev_region(dev, 1);

    pr_info("Sysfs char driver unloaded\n");
}

module_init(my_driver_init);
module_exit(my_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION(" sysfs character device driver");
MODULE_VERSION("1.0");

