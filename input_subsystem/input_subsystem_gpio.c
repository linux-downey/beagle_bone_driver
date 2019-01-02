#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/slab.h>
#include <linux/regmap.h>
#include <linux/input.c>

struct input_dev *gpio_dev;

#define BUTTON0   26
#define BUTTON1   27

int button_irq_num0,button_irq_num1;

static int gpio_input_init(void)
{

    /*分配input_dev结构体*/
    gpio_dev = input_allocate_device();
    /*设置*/
    set_bit(EV_KEY,gpio_dev->evbit);
    set_bit(KEY_L,gpio_dev->keybit);
    set_bit(KEY_S,gpio_dev->keybit);
    /*注册*/
    input_register_device(gpio_dev);
    /*硬件相关操作*/
    button_irq_num0 = gpio_to_irq(BUTTON0);
    ret = request_irq(button_irq_num0,
                            (irq_handler_t)button_irq_handle0,
                            IRQF_TRIGGER_RISING,
                            "BUTTON0",
                            NULL);
    button_irq_num1 = gpio_to_irq(BUTTON1);
    ret = request_irq(button_irq_num1,
                            (irq_handler_t)button_irq_handle1,
                            IRQF_TRIGGER_RISING,
                            "BUTTON1",
                            NULL);
    return 0;
}

static void gpio_input_exit(void)
{

}

module_init(gpio_input_init);
module_exit(gpio_input_exit);

MODULE_LICENSE("GPL");

