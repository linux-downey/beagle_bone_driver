#include <linux/init.h>  
#include <linux/module.h>
#include <linux/device.h>  
#include <linux/kernel.h>  
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/string.h>


MODULE_AUTHOR("Downey");
MODULE_LICENSE("GPL");

static int majorNumber = 0;
/*Class 名称，对应/sys/class/下的目录名称*/
static const char *CLASS_NAME = "led_control_class";
/*Device 名称，对应/dev下的目录名称*/
static const char *DEVICE_NAME = "led_control_demo";

static int led_control_open(struct inode *node, struct file *file);
static ssize_t led_control_read(struct file *file,char *buf, size_t len,loff_t *offset);
static ssize_t led_control_write(struct file *file,const char *buf,size_t len,loff_t* offset);
static int led_control_release(struct inode *node,struct file *file);

#define LED_PIN   26 
static int gpio_status;


static char recv_msg[20];

static struct class *led_control_class = NULL;
static struct device *led_control_device = NULL;

/*File opertion 结构体，我们通过这个结构体建立应用程序到内核之间操作的映射*/
static struct file_operations file_oprts = 
{
    .open = led_control_open,
    .read = led_control_read,
    .write = led_control_write,
    .release = led_control_release,
};


static void gpio_config(void)
{
    if(!gpio_is_valid(LED_PIN)){
        printk(KERN_ALERT "Error wrong gpio number\n");
        return ;
    }
    gpio_request(LED_PIN,"led_ctr");
    gpio_direction_output(LED_PIN,1);
	gpio_export(LED_PIN,false);
    gpio_set_value(LED_PIN,1);
    gpio_status = 1;
}


static void gpio_deconfig(void)
{
    gpio_free(LED_PIN);
}

static int __init led_control_init(void)
{
    printk(KERN_ALERT "Driver init\r\n");
    /*注册一个新的字符设备，返回主设备号*/
    majorNumber = register_chrdev(0,DEVICE_NAME,&file_oprts);
    if(majorNumber < 0 ){
        printk(KERN_ALERT "Register failed!!\r\n");
        return majorNumber;
    }
    printk(KERN_ALERT "Registe success,major number is %d\r\n",majorNumber);

    /*以CLASS_NAME创建一个class结构，这个动作将会在/sys/class目录创建一个名为CLASS_NAME的目录*/
    led_control_class = class_create(THIS_MODULE,CLASS_NAME);
    if(IS_ERR(led_control_class))
    {
        unregister_chrdev(majorNumber,DEVICE_NAME);
        return PTR_ERR(led_control_class);
    }

    /*以DEVICE_NAME为名，参考/sys/class/CLASS_NAME在/dev目录下创建一个设备：/dev/DEVICE_NAME*/
    led_control_device = device_create(led_control_class,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
    if(IS_ERR(led_control_device))
    {
        class_destroy(led_control_class);
        unregister_chrdev(majorNumber,DEVICE_NAME);
        return PTR_ERR(led_control_device);
    }
    printk(KERN_ALERT "led_control device init success!!\r\n");

    return 0;
}

/*当用户打开这个设备文件时，调用这个函数*/
static int led_control_open(struct inode *node, struct file *file)
{
    printk(KERN_ALERT "GPIO init \n");
    gpio_config();
    return 0;
}

/*当用户试图从设备空间读取数据时，调用这个函数*/
static ssize_t led_control_read(struct file *file,char *buf, size_t len,loff_t *offset)
{
    int cnt = 0;
    /*将内核空间的数据copy到用户空间*/
    cnt = copy_to_user(buf,&gpio_status,1);
    if(0 == cnt){
        return 0;
    }
    else{
        printk(KERN_ALERT "ERROR occur when reading!!\n");
        return -EFAULT;
    }
    return 1;
}

/*当用户往设备文件写数据时，调用这个函数*/
static ssize_t led_control_write(struct file *file,const char *buf,size_t len,loff_t *offset)
{
    /*将用户空间的数据copy到内核空间*/
    int cnt = copy_from_user(recv_msg,buf,len);
    if(0 == cnt){
        if(0 == memcmp(recv_msg,"on",2))
        {
            printk(KERN_INFO "LED ON!\n");
            gpio_set_value(LED_PIN,1);
            gpio_status = 1;
        }
        else
        {
            printk(KERN_INFO "LED OFF!\n");
            gpio_set_value(LED_PIN,0);
            gpio_status = 0;
        }
    }
    else{
        printk(KERN_ALERT "ERROR occur when writing!!\n");
        return -EFAULT;
    }
    return len;
}

/*当用户打开设备文件时，调用这个函数*/
static int led_control_release(struct inode *node,struct file *file)
{
    printk(KERN_INFO "Release!!\n");
	gpio_unexport(LED_PIN);
    gpio_deconfig();
    return 0;
}

/*销毁注册的所有资源，卸载模块，这是保持linux内核稳定的重要一步*/
static void __exit led_control_exit(void)
{
    device_destroy(led_control_class,MKDEV(majorNumber,0));
    class_unregister(led_control_class);
    class_destroy(led_control_class);
    unregister_chrdev(majorNumber,DEVICE_NAME);
}

module_init(led_control_init);
module_exit(led_control_exit);