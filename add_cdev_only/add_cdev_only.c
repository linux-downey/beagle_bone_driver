#include <linux/init.h>  
#include <linux/module.h>
#include <linux/device.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/timer.h>

MODULE_AUTHOR("Downey");
MODULE_LICENSE("GPL");

int majorNumber;
dev_t dev_id;
struct cdev basic_cdev;
struct class *basic_class;
struct device *basic_device;
struct basic_struct{
	struct wait_queue_head wq;
	struct timer_list timer;
	int flag;
};

static const char *CLASS_NAME = "basic_class";
static const char *DEVICE_NAME = "basic_demo";


void timer_cb(unsigned long data)
{
    
    printk("timer_callback\n");
    struct basic_struct *basic = (struct basic_struct*)data;
    if(NULL == basic) {
    	printk("basic = null!\n");

    	return;
    }
    basic->flag = 1;
    wake_up_all(&basic->wq);
}

static int basic_open(struct inode *node, struct file *file)
{
    printk("open!\n");
    struct basic_struct *basic = kmalloc(sizeof(struct basic_struct),GFP_KERNEL);
    if(IS_ERR(basic))
	return -ENOMEM;
    basic->flag = 0;

    init_timer(&basic->timer);
    basic->timer.function = timer_cb;
    basic->timer.expires = jiffies + 3 * HZ;
    basic->timer.data = (unsigned long)basic;
    add_timer(&basic->timer);
    
    init_waitqueue_head(&basic->wq);
    
    file->private_data = basic;

    return 0;
}

static ssize_t basic_read(struct file *file,char *buf, size_t len,loff_t *offset)
{
	
    struct basic_struct *basic = (struct basic_struct*)file->private_data;
    if(basic->flag != 1){
	    printk("wait for flag!\n");
	    wait_event_interruptible(basic->wq,basic->flag);
	    if(1 == basic->flag){
		basic->flag = 0;
	    	printk("wait for flag over!\n");
	    }
    }
    return 0;
}

static ssize_t basic_write(struct file *file,const char *buf,size_t len,loff_t *offset)
{
    return len;
}

static int basic_release(struct inode *node,struct file *file)
{
    printk("close!\n");
    struct basic_struct *basic = (struct basic_struct*)file->private_data;

    del_timer(&basic->timer);
    kfree(basic);
    return 0;
}

static struct file_operations file_oprts = 
{
    .open = basic_open,
    .read = basic_read,
    .write = basic_write,
    .release = basic_release,
};


static int __init basic_init(void)
{
    int ret;
    printk(KERN_ALERT "Driver init\r\n");
    ret = alloc_chrdev_region(&dev_id,0,2,DEVICE_NAME);
    if(ret){

        printk(KERN_ALERT "Alloc chrdev region failed!!\r\n");
        return -ENOMEM;
    }
    majorNumber = MAJOR(dev_id);
    cdev_init(&basic_cdev,&file_oprts);
    ret = cdev_add(&basic_cdev,dev_id,2);

    basic_class = class_create(THIS_MODULE,CLASS_NAME);
    basic_device = device_create(basic_class,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
    printk(KERN_INFO "major = %d,mino = %d\n",MAJOR(dev_id),MINOR(dev_id));
    printk(KERN_ALERT "Basic device init success!!\r\n");

    return 0;
}

static void __exit basic_exit(void)
{
    device_destroy(basic_class,MKDEV(majorNumber,0));
    class_unregister(basic_class);
    class_destroy(basic_class);
    cdev_del(&basic_cdev);
    unregister_chrdev_region(dev_id,2);
}

module_init(basic_init);
module_exit(basic_exit);
