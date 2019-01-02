#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#define BUTTON     27
#define LED        26

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Downey");
MODULE_DESCRIPTION("Gpio irq test!!\n");

int button_irq_num = 0;
bool led_status = 1;


static  irqreturn_t button_irq_handle(int irq, void *dev_id)
{
    printk(KERN_INFO "Enter irq!!\n");
    if(0 == led_status)
    {
        gpio_set_value(LED,1);
        led_status = 1;
    }
    else
    {
        led_status = 0;
        gpio_set_value(LED,0);
    }
    return (irqreturn_t)IRQ_HANDLED;
}


static int gpio_config(void)
{
    int ret = 0;
    if(!gpio_is_valid(BUTTON) || !gpio_is_valid(LED))
    {
        printk(KERN_ALERT "Gpio is invalid!\n");
        return -ENODEV;
    }
    gpio_request(BUTTON,"button");
    gpio_direction_input(BUTTON);
    gpio_set_debounce(BUTTON,20);

    button_irq_num = gpio_to_irq(BUTTON);
    printk(KERN_INFO "NUM = %d",button_irq_num);
    ret = request_irq(button_irq_num,
                            (irq_handler_t)button_irq_handle,
                            IRQF_TRIGGER_RISING,
                            "BUTTON1",
                            NULL);
    printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", ret);
    
    gpio_request(LED,"LED");
    gpio_direction_output(LED,1);
    gpio_set_value(LED,1);
    return 0;
}


static void gpio_deconfig(void)
{
    gpio_direction_output(LED,0);
    free_irq(button_irq_num,NULL);
    gpio_free(BUTTON);
    gpio_free(LED);
}

int __init gpio_irq_init(void)
{
    gpio_config();
    printk(KERN_INFO "gpio_irq_init!\n");
    return 0;
}


void __exit gpio_irq_exit(void)
{
    gpio_deconfig();
    printk(KERN_INFO "gpio_irq_exit!\n");
}

module_init(gpio_irq_init);
module_exit(gpio_irq_exit);