#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <linux/interrupt.h>
#include <linux/of_irq.h>
#include <linux/input.h>

MODULE_LICENSE("GPL");

#define TIME_TRIG                   (15)

struct local_dev_t {
    struct input_dev *inputdev;
    struct device_node	*nd;
    unsigned int key_gpio;
    struct timer_list timer;
    int key_stat;
    spinlock_t lock;
    int irq;
};

enum key_stat{
    KEY_PRESS = 0,
    KEY_RELEASE,
    KEY_KEEP
};

static struct local_dev_t local_dev;

irqreturn_t key_handle(int irq, void *dev_id)
{
    struct local_dev_t * locdev = dev_id;
    mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(TIME_TRIG));
    return IRQ_HANDLED;
}

/* For 按键消抖 */
static void timer_callback(struct timer_list * tm)
{
    static int cur_stat = 0, last_stat = 0, stat = KEY_KEEP;
    unsigned long flags;
    struct local_dev_t * locdev;
    locdev = container_of(tm, struct local_dev_t, timer);

    /* Update key status */
    cur_stat = gpio_get_value(locdev->key_gpio);
    if (cur_stat != last_stat) {
        stat = (cur_stat == 0)?KEY_RELEASE:KEY_PRESS;
    }
    else{
        stat = KEY_KEEP;
    }
    last_stat = cur_stat;
    spin_lock_irqsave(&locdev->lock, flags);
    locdev->key_stat = stat;
    spin_unlock_irqrestore(&locdev->lock, flags);
    
    /* 上报input事件 */
    input_report_key(locdev->inputdev, KEY_1, locdev->key_stat);
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

    ret = request_irq(dev->irq, key_handle, irq_type, "key-irq", dev);
    if (ret) {
		printk("irq: Failed to request_irq, ret=%d\n", ret);
    }
    return ret;
}

static int get_gpio_msg(struct local_dev_t *dev)
{
    int ret = 0;

    dev->key_gpio = of_get_named_gpio(dev->nd, "keys-gpios", 0);
	if(dev->key_gpio < 0) {
		printk("can't get gpios");
		return -EINVAL;
	}
    ret = gpio_request(dev->key_gpio, "KEY-GPIO");
    if (ret) {
        printk(KERN_ERR "gpiokey: Failed to request gpios\n");
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
int hello_probe(struct platform_device *pdev)
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

    ret = get_gpio_msg(local_dev);
    if (ret)
        goto free_localdev;
    timer_setup(&local_dev->timer, timer_callback, 0);
    ret = key_set_irq(local_dev);
    if (ret)
        goto free_gpio;;

    /* Start init input-device */
    inputdev = input_allocate_device();
	if (!inputdev) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Not enough memory.\n", __func__);
		goto free_irq;
	}
    /* 设置要上报的事件 */
    set_bit(EV_KEY, inputdev->evbit);
    set_bit(EV_REP, inputdev->evbit);
	set_bit(KEY_1, inputdev->keybit);

	inputdev->name = "Input Button";

    ret = input_register_device(inputdev);
    if (ret) {
		printk(KERN_ERR "%s: Failed to register device\n", __func__);
		goto free_inputdev;
	}
    local_dev->inputdev = inputdev;
    /* 把local_dev放到pdev->dev.drvdata中 */
    platform_set_drvdata(pdev, &local_dev);

    return 0;

free_inputdev:
    input_free_device(inputdev);
free_irq:
    free_irq(local_dev->irq, &local_dev);
free_gpio:
    gpio_free(local_dev->key_gpio);
    del_timer_sync(&local_dev->timer);
free_localdev:
    devm_kfree(&pdev->dev, local_dev);
out:
    return ret;
}
/* 驱动或设备一方卸载后调用 */
int hello_remove(struct platform_device *pdev)
{
    struct local_dev_t *localdev = pdev->dev.driver_data;

    printk("driver key remove \n");
    gpio_free(localdev->key_gpio);
    del_timer_sync(&localdev->timer);
    free_irq(localdev->irq, &local_dev);
    input_free_device(localdev->inputdev);
    devm_kfree(&pdev->dev, localdev);
    /* 需要unregister input吗 */
    return 0;
}

static const struct of_device_id local_match[] = {
	{ .compatible   = "eden,key" },         /* 根据此属性和设备树上的compatible进行匹配*/
	{},
};
MODULE_DEVICE_TABLE(of, local_match);
struct platform_driver hello_driver = {
    .driver = {
		.name		    = "key_input_driver",   /* display in /sys/bus/platform/drivers */            
		.of_match_table = of_match_ptr(local_match),
	},
    .probe = hello_probe,
    .remove = hello_remove,
};

/* 等同于在init和exit中仅仅注册注销驱动 */
module_platform_driver(hello_driver);

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register input_device key"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名
