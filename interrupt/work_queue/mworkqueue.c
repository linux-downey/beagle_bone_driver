#include <linux/init.h>             
#include <linux/module.h>          
#include <linux/kernel.h>   
#include <linux/workqueue.h>

MODULE_LICENSE("GPL");              



struct wq_priv{
	struct work_struct work;
	struct delayed_work dl_work;
};


void work_func(struct work_struct *work){
	printk("exec work queue!\n");
}
void dl_work_func(struct work_struct *work){
	printk("exec delayed work queue!\n");
}

//DECLARE_WORK(work, work_func);

struct wq_priv priv;

static int __init workqueue_init(void)
{
	printk("hello world!!!\n");

	INIT_WORK(&priv.work,work_func);
	INIT_DELAYED_WORK(&priv.dl_work,dl_work_func);

	//queue_work(system_wq,&work);
	if(0 == schedule_work(&priv.work)){
		printk("Failed to run workqueue!\n");
	}
	if(0 == schedule_delayed_work(&priv.dl_work,3*HZ)){
		printk("Failed to run workqueue!\n");
	}
	//cancel_delayed_work_sync(&priv.dl_work);
	flush_delayed_work(&priv.dl_work);
	printk("after??????????\n");
	return 0;
}


static void __exit workqueue_exit(void)
{
	printk("goodbye world!!!\n");
	flush_work(&priv.work);
	flush_delayed_work(&priv.dl_work);
	cancel_work_sync(&priv.work);
	cancel_delayed_work_sync(&priv.dl_work);
	//cancel_work(&priv.work);
}


module_init(workqueue_init);
module_exit(workqueue_exit);
