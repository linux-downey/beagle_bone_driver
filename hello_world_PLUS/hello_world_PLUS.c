#include <linux/init.h>             
#include <linux/module.h>          
#include <linux/kernel.h>   

MODULE_AUTHOR("Downey");      //作者信息
MODULE_DESCRIPTION("Linux kernel driver - hello_world PLUS!");  //模块的描述，可以使用modinfo xxx.ko指令来查看
MODULE_VERSION("0.1");             //模块版本号
//指定license版本
MODULE_LICENSE("GPL");              

static char *name = "world";
module_param(name,charp,S_IRUGO);                                 //设置加载时可传入的参数
MODULE_PARM_DESC(name,"type: char *,permission: S_IRUGO");        //参数描述信息

//设置初始化入口函数
static int __init hello_world_init(void)
{
	printk(KERN_DEBUG "hello %s!!!\n",name);
	return 0;
}

//设置出口函数
static void __exit hello_world_exit(void)
{
	printk(KERN_DEBUG "goodbye %s!!!\n",name);
}

//将上述定义的init()和exit()函数定义为模块入口/出口函数
module_init(hello_world_init);
module_exit(hello_world_exit);