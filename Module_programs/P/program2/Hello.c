#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

int x=250;
char name[20]= "universe";

static void function1(void);

static int __init hello_init(void)
{
	printk(KERN_INFO "Module init function HELLO \n");
	function1();
	return 0;
}

static void __exit hello_exit(void)
{

	printk(KERN_INFO "Module exit function HELLO \n");
}


static void function1(void)
{

        printk(KERN_INFO "value x=%d\n",x);
        printk(KERN_INFO "name =%s\n",name);
	
}


module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION("Simple Hello and value inserted Module");
