#include <linux/init.h>  
#include <linux/module.h>
#include <linux/device.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>

MODULE_AUTHOR("Downey");
MODULE_LICENSE("GPL");

static int majorNumber = 0;
/*Class 名称，对应/sys/class/下的目录名称*/
static const char *CLASS_NAME = "basic_class";
/*Device 名称，对应/dev下的目录名称*/
static const char *DEVICE_NAME = "basic_demo";

static int basic_open(struct inode *node, struct file *file);
static ssize_t basic_read(struct file *file,char *buf, size_t len,loff_t *offset);
static ssize_t basic_write(struct file *file,const char *buf,size_t len,loff_t* offset);
static int basic_release(struct inode *node,struct file *file);


static char msg[] = "Downey!";
static char recv_msg[20];

static struct class *basic_class = NULL;
static struct device *basic_device = NULL;

/*File opertion 结构体，我们通过这个结构体建立应用程序到内核之间操作的映射*/
static struct file_operations file_oprts = 
{
    .open = basic_open,
    .read = basic_read,
    .write = basic_write,
    .release = basic_release,
};

static dev_t dev_id = 0;
static int ret = 0;
static struct cdev basic_cdev;

static int __init basic_init(void)
{
    printk(KERN_ALERT "Driver init\r\n");
    /*创建设备号，如果主设备号已被指定，就需要指定次设备号*/
    if(majorNumber){
        dev_id = MKDEV(majorNumber,0);
        ret = register_chrdev_region(dev_id,2,DEVICE_NAME);
        if(ret){
            printk(KERN_ALERT "Register chrdev region failed!!\r\n");
            return -ENOMEM;
        }
    }
    /*如果没有指定主设备号，就申请一个主设备号*/
    else{
        ret = alloc_chrdev_region(&dev_id,0,2,DEVICE_NAME);
        if(ret){

            printk(KERN_ALERT "Alloc chrdev region failed!!\r\n");
            return -ENOMEM;
        }
        majorNumber = MAJOR(dev_id);
    }
    /*初始化一个cdev*/
    cdev_init(&basic_cdev,&file_oprts);
    /*添加一个cdev*/
    ret = cdev_add(&basic_cdev,dev_id,2);
    basic_class = class_create(THIS_MODULE,CLASS_NAME);
    /*新创建一个设备节点*/
    basic_device = device_create(basic_class, NULL, MKDEV(majorNumber,0), NULL, DEVICE_NAME);
    printk(KERN_ALERT "Basic device init success!!\r\n");

    return 0;
}

/*当用户打开这个设备文件时，调用这个函数*/
static int basic_open(struct inode *node, struct file *file)
{
    printk(KERN_ALERT "Open file\r\n");
    return 0;
}

/*当用户试图从设备空间读取数据时，调用这个函数*/
static ssize_t basic_read(struct file *file,char *buf, size_t len,loff_t *offset)
{
    int cnt = 0;
    /*将内核空间的数据copy到用户空间*/
    cnt = copy_to_user(buf,msg,sizeof(msg));
    if(0 == cnt){
        printk(KERN_INFO "Send file!!\n");
        return 0;
    }
    else{
        printk(KERN_ALERT "ERROR occur when reading!!\n");
        return -EFAULT;
    }
    return sizeof(msg);
}

/*当用户往设备文件写数据时，调用这个函数*/
static ssize_t basic_write(struct file *file,const char *buf,size_t len,loff_t *offset)
{
    /*将用户空间的数据copy到内核空间*/
    int cnt = copy_from_user(recv_msg,buf,len);
    if(0 == cnt){
        printk(KERN_INFO "Recieve file!!\n");
    }
    else{
        printk(KERN_ALERT "ERROR occur when writing!!\n");
        return -EFAULT;
    }
    printk(KERN_INFO "Recive data ,len = %s\n",recv_msg);
    return len;
}

/*当用户打开设备文件时，调用这个函数*/
static int basic_release(struct inode *node,struct file *file)
{
    printk(KERN_INFO "Release!!\n");
    return 0;
}

/*销毁注册的所有资源，卸载模块，这是保持linux内核稳定的重要一步*/
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