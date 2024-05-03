#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/input.h>

/*
本例程使用中断发送input事件，上报按键按下的状态

设备树设置：
{
    key {
            compatible = "eden,key";
            pinctrl-names = "eden,key_default";
            pinctrl-0 = <&key_pins>;
            keys-gpios = <&gpio2 RK_PD4 GPIO_ACTIVE_HIGH>;
            interrupt-parent = <&gpio2>;
            interrupts = <28 IRQ_TYPE_EDGE_BOTH>;
            status = "okay";
        };
}

查看设备节点信息
udevadm info /dev/input/event3
查看事件（本驱动用这个测试）
evtest /dev/input/event3
查看中断触发的次数
cat /proc/interrupts | grep key-irq
查看gpio状态
cat /sys/kernel/debug/gpio | grep KEY-GPIO
*/

MODULE_LICENSE("GPL");

#define TIME_TRIG                   (15)

struct key_stat_t{
    int cur_stat;
    int last_stat;
    int stat;
};

struct local_dev_t {
    struct input_dev *inputdev;
    struct device_node	*nd;
    unsigned int key_gpio;
    struct timer_list timer;
    struct key_stat_t key_stat;
    spinlock_t lock;
    int irq;
};

enum key_stat_em{
    KEY_RELEASE = 0,
    KEY_PRESS,
    KEY_KEEP
};

irqreturn_t key_handle(int irq, void *dev_id)
{
    struct local_dev_t * locdev = dev_id;
    mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(TIME_TRIG));
    return IRQ_HANDLED;
}

/* For 按键消抖 */
static void timer_callback(struct timer_list * tm)
{
    struct key_stat_t *key;
    unsigned long flags;
    struct local_dev_t * locdev;
    int stat;
    locdev = container_of(tm, struct local_dev_t, timer);

    {/* 更新按键状态 */
    spin_lock_irqsave(&locdev->lock, flags);
    key = &locdev->key_stat;
    key->cur_stat = (gpio_get_value(locdev->key_gpio) == 0)?KEY_RELEASE:KEY_PRESS;
    if (key->cur_stat != key->last_stat) {
        key->stat = key->cur_stat;
    }
    else{
        key->stat = KEY_KEEP;
    }
    key->last_stat = key->cur_stat;
    stat = key->stat;
    spin_unlock_irqrestore(&locdev->lock, flags);
    }
    
    if(stat == KEY_KEEP)
        return;

    /* 只在按键按下和释放时上报input事件 */
    input_report_key(locdev->inputdev, BTN_1, stat);
	input_sync(locdev->inputdev);
}

static int key_set_irq(struct local_dev_t *dev)
{
    u32 irq_type, ret = 0;

    dev->irq = irq_of_parse_and_map(dev->nd, 0);
    if(!dev->irq) {
		printk("irq: Failed to get parse\n");
		return -EINVAL;
	}
    irq_type = irq_get_trigger_type(dev->irq);
    if ((irq_type & IRQ_TYPE_SENSE_MASK) == IRQ_TYPE_NONE) {
        irq_type |= IRQ_TYPE_EDGE_BOTH;
    }
    /* cat /proc/interrupts | grep key-irq */
    ret = request_irq(dev->irq, key_handle, irq_type, "key-irq", dev);
    if (ret) {
		printk("irq: Failed to request_irq, ret=%d\n", ret);
    }
    return ret;
}

static int get_gpio_msg(struct local_dev_t *dev)
{ 
    int ret = 0;

    ret = of_get_named_gpio(dev->nd, "keys-gpios", 0);
	if(ret < 0) {
		printk("can't get gpios\r\n");
		return -EINVAL;
	}
    dev->key_gpio = ret;
    /* cat /sys/kernel/debug/gpio | grep KEY-GPIO */
    ret = gpio_request(dev->key_gpio, "KEY-GPIO");
    if (ret) {
        printk(KERN_ERR "gpiokey: Failed to request gpios\r\n");
        return ret;
	}

	ret = gpio_direction_input(dev->key_gpio);
	if(ret < 0) {
		printk("can't set gpio to input!\r\n");
        gpio_free(dev->key_gpio);
	}

    return ret;
}

/* 匹配成功后调用 */
int inputkey_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct local_dev_t *local_dev;
    struct input_dev *inputdev;

    printk("driver key probe \n");
    local_dev = devm_kzalloc(&pdev->dev, sizeof(*local_dev), GFP_KERNEL);
    if (!local_dev) {
        ret = ENOMEM;
        goto out;
    }
    
    local_dev->nd = pdev->dev.of_node;

    {/* 初始化硬件、中断*/
    ret = get_gpio_msg(local_dev);
    if (ret)
        goto free_localdev;
    timer_setup(&local_dev->timer, timer_callback, 0);
    ret = key_set_irq(local_dev);
    if (ret)
        goto free_gpio;
    }

    /* 分配设备，可能会陷入睡眠 */
    inputdev = input_allocate_device();
	if (!inputdev) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Not enough memory.\n", __func__);
		goto free_irq;
	}
    /* 设置要上报的事件 */
    set_bit(EV_KEY, inputdev->evbit);
    /* 用于上报按键持续按下的情况 */
    set_bit(EV_REP, inputdev->evbit);
	set_bit(BTN_1, inputdev->keybit);

	inputdev->name = "Input Button";

    ret = input_register_device(inputdev);
    if (ret) {
		printk(KERN_ERR "%s: Failed to register device\n", __func__);
		goto free_inputdev;
	}
    local_dev->inputdev = inputdev;
    /* 把local_dev放到pdev->dev.drvdata中 */
    platform_set_drvdata(pdev, local_dev);

    return 0;

free_inputdev:
    /* 在设备没有被注册的情况下使用，注册后则使用input_unregister_device() */
    input_free_device(inputdev);
free_irq:
    free_irq(local_dev->irq, local_dev);
free_gpio:
    gpio_free(local_dev->key_gpio);
    del_timer_sync(&local_dev->timer);
free_localdev:
    devm_kfree(&pdev->dev, local_dev);
out:
    return ret;
}
/* 驱动或设备一方卸载后调用 */
int inputkey_remove(struct platform_device *pdev)
{
    struct local_dev_t *localdev = platform_get_drvdata(pdev);

    gpio_free(localdev->key_gpio);
    del_timer_sync(&localdev->timer);
    free_irq(localdev->irq, localdev);
    input_unregister_device(localdev->inputdev);
    return 0;
}

static const struct of_device_id local_match[] = {
	{ .compatible   = "eden,key" },         /* 根据此属性和设备树上的compatible进行匹配*/
	{},
};
MODULE_DEVICE_TABLE(of, local_match);
struct platform_driver keyinput_driver = {
    .driver = {
		.name		    = "key_input_driver",   /* display in /sys/bus/platform/drivers */            
		.of_match_table = of_match_ptr(local_match),
	},
    .probe = inputkey_probe,
    .remove = inputkey_remove,
};

/* 等同于在init和exit中仅仅注册注销驱动 */
module_platform_driver(keyinput_driver);

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register input_device key"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名
