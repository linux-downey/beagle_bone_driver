#include <linux/init.h>             
#include <linux/module.h>          
#include <linux/kernel.h>   
#include <linux/workqueue.h>
#include <linux/cpumask.h>

//指定license版本
MODULE_LICENSE("GPL");   


//设置初始化入口函数
static int __init hello_world_init(void)
{
	int i=0,cpu=0;
	for_each_possible_cpu(cpu){
		i++;
	}
	printk("i = %d\n",i);
	printk("sizeof(__cpu_possible_mask.bits) = %d\n",sizeof(__cpu_possible_mask.bits));
	printk("NR_CPUS = %d\n",NR_CPUS);
	
	return 0;
}

//设置出口函数
static void __exit hello_world_exit(void)
{
	int i;
	printk(KERN_DEBUG "goodbye world!!!\n");
}

//将上述定义的init()和exit()函数定义为模块入口/出口函数
module_init(hello_world_init);
module_exit(hello_world_exit);
