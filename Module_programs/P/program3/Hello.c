#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>

int x=250;
char name[20]= "universe";

static void function1(void);
static int add(int,int);

static int __init hello_init(void)
{
	printk(KERN_INFO "Module init function HELLO \n");
	int a=5,b=10;
	function1();
	int c = add(a,b);
	printk(KERN_INFO "Addition value c=%d\n",c);
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


static int add(int a, int b)
{
	int z = a+b;
	return z;
}



module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santhosh");
MODULE_DESCRIPTION("Simple Add function in kernel space Module");
