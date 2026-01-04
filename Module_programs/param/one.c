// module parameter dynamically can insert a value 


#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/moduleparam.h>
 
int valueX;

 
module_param(valueX, int, S_IRUSR|S_IWUSR); 

/*
** Module init function
*/
static int __init one_init(void)
{
        int i;
        printk(KERN_INFO "ValueX = %d  \n", valueX);
        
        printk(KERN_INFO "Kernel Module Inserted Successfully...\n");
    return 0;
}

/*
** Module Exit function
*/
static void __exit one_exit(void)
{
    printk(KERN_INFO "Kernel Module Removed Successfully...\n");
}
 
module_init(one_init);
module_exit(one_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("santhosh");
MODULE_DESCRIPTION("A simple module parameter driver");
MODULE_VERSION("1.0");
