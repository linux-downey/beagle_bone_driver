#include <linux/export.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/signal.h>
#include <linux/completion.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include <linux/cpu.h>
#include <linux/notifier.h>
#include <linux/kthread.h>
#include <linux/hardirq.h>
#include <linux/mempolicy.h>
#include <linux/freezer.h>
#include <linux/kallsyms.h>
#include <linux/debug_locks.h>
#include <linux/lockdep.h>
#include <linux/idr.h>
#include <linux/jhash.h>
#include <linux/hashtable.h>
#include <linux/rculist.h>
#include <linux/nodemask.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <linux/nmi.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/sections.h>
#include <asm/cacheflush.h>
#include <asm/sections.h>
#include <asm/tlbflush.h>
#include <asm/io.h>

#include <linux/bitmap.h>
#include <linux/err.h>
#include <linux/lcm.h>
#include <linux/list.h>
#include <linux/log2.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/percpu.h>
#include <linux/pfn.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/vmalloc.h>
#include <linux/workqueue.h>
#include <linux/kmemleak.h>
#include <linux/sched.h>

static struct task_struct *task1;
static struct task_struct *task2;
static struct task_struct *task3;
static struct task_struct *task4;
static spinlock_t lock;

MODULE_LICENSE("GPL");    

DEFINE_PER_CPU(int,val);

int *pptr = NULL;

static int task1_func(void *arg){

	int *value = NULL;
	spin_lock(&lock);
	printk("This is task1\n");
	printk("The cpu id = %d\n",raw_smp_processor_id());
	printk("The cpu possible mask = 0x%lx\n",__cpu_possible_mask.bits[0]);
	printk("The cpu online mask = 0x%lx\n",__cpu_online_mask.bits[0]);
	printk("The cpu present mask = 0x%lx\n",__cpu_present_mask.bits[0]);
	printk("The cpu active mask = 0x%lx\n",__cpu_active_mask.bits[0]);

	int *p=per_cpu_ptr(pptr,1);
	*p += 1;
	printk("percpu pptr addr = 0x%lx\n",(unsigned long)per_cpu_ptr(pptr,raw_smp_processor_id()));

	spin_unlock(&lock);
	while(1){
		msleep(100);
		if(kthread_should_stop())
			return 0;
	}
	return 0;
}

static int task2_func(void *arg){

	int *value = NULL;
	spin_lock(&lock);
	printk("This is task2\n");
	printk("The cpu id = %d\n",raw_smp_processor_id());

	printk("percpu pptr addr = 0x%lx\n",(unsigned long)per_cpu_ptr(pptr,raw_smp_processor_id()));
	
	printk("percpu pptr value = %d\n",*per_cpu_ptr(pptr,raw_smp_processor_id()));
	spin_unlock(&lock);
	while(1){
		msleep(100);
		if(kthread_should_stop())
			return 0;
	}
	return 0;
}

static int task3_func(void *arg){

	int *value = NULL;
	spin_lock(&lock);
	printk("This is task3\n");
	printk("The cpu id = %d\n",raw_smp_processor_id());
	
	printk("percpu pptr addr = 0x%lx\n",(unsigned long)per_cpu_ptr(pptr,raw_smp_processor_id()));
	
	printk("percpu pptr addr = %d\n",*per_cpu_ptr(pptr,raw_smp_processor_id()));
	spin_unlock(&lock);
	while(1){
		msleep(100);
		if(kthread_should_stop())
			return 0;
	}
	return 0;
}


static int task4_func(void *arg){

	int *value = NULL;
	spin_lock(&lock);
	printk("This is task4\n");
	printk("The cpu id = %d\n",raw_smp_processor_id());
	
	printk("percpu pptr pptr = 0x%lx\n",(unsigned long)per_cpu_ptr(pptr,raw_smp_processor_id()));
	printk("percpu pptr value = %d\n",*per_cpu_ptr(pptr,raw_smp_processor_id()));
	
	spin_unlock(&lock);
	while(1){
		msleep(100);
		if(kthread_should_stop())
			return 0;
	}
	return 0;
}

static int __init hello_world_init(void)
{
	printk("\n\n");
	
	pptr = alloc_percpu(int);

	printk("percpu ptr addr = 0x%lx\n",(unsigned long)pptr);

	spin_lock_init(&lock);
	task1 = kthread_create(task1_func,NULL,"task1");
	if(IS_ERR(task1)){printk("error creating thread");}
	task2 = kthread_create(task2_func,NULL,"task2");
	if(IS_ERR(task2)){printk("error creating thread");}
	task3 = kthread_create(task3_func,NULL,"task3");
	if(IS_ERR(task3)){printk("error creating thread");}
	task4 = kthread_create(task4_func,NULL,"task4");
	if(IS_ERR(task4)){printk("error creating thread");}

	kthread_bind(task1,0);
	kthread_bind(task2,1);
	kthread_bind(task3,0);
	kthread_bind(task4,1);

	wake_up_process(task1);
	wake_up_process(task2);
	wake_up_process(task3);
	wake_up_process(task4);

	return 0;
}

static void __exit hello_world_exit(void)
{
	kthread_stop(task1);
	kthread_stop(task2);
	kthread_stop(task3);
	kthread_stop(task4);

	free_percpu(pptr);
	printk("\n\n");
//	printk(KERN_DEBUG "goodbye world!!!\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);
