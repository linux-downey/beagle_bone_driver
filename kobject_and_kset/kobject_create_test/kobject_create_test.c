#include <linux/init.h>             
#include <linux/module.h>          
#include <linux/kernel.h>   
#include <linux/kobject.h>
#include <linux/sysfs.h>
#include <linux/slab.h>

//指定license版本
MODULE_LICENSE("GPL");              

static struct kobject *kob;
static struct kset *kst;

//设置初始化入口函数
static int __init hello_world_init(void)
{

    int ret = 0;
	kst = kset_create_and_add("test_kset",NULL,kernel_kobj->parent);
	if(!kst)
	{
		printk(KERN_ALERT "Create kset failed\n");
		kset_put(kst);
	}
    kob = kzalloc(sizeof(*kob),GFP_KERNEL);
    if(IS_ERR(kob)){
        printk(KERN_ALERT "alloc failed!!\n");
        return -ENOMEM;
    }
	
    ret = kobject_init_and_add(kob, NULL, NULL, "%s", "test_obj");
    if(ret)
    {
        kobject_put(kob);
		kset_unregister(kst);
    }

	printk(KERN_DEBUG "kobj test project!!!\n");
	return 0;
}

//设置出口函数
static void __exit hello_world_exit(void)
{
	kobject_put(kob);
	kset_unregister(kst);
	printk(KERN_DEBUG "goodbye !!!\n");
}

//将上述定义的init()和exit()函数定义为模块入口/出口函数
module_init(hello_world_init);
module_exit(hello_world_exit);