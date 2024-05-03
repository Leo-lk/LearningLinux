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
/* 按键有有电平变化，中断触发*/
irqreturn_t key_handle(int irq, void *dev_id)
{
    struct local_dev_t * locdev = dev_id;
    /* 启动定时器*/
    mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(TIME_TRIG));
    return IRQ_HANDLED;
}

/* 定时器回调函数 */
static void timer_callback(struct timer_list * tm)
{
    static int cur_stat = 0, last_stat = 0, stat = KEY_KEEP;
    unsigned long flags;
    struct local_dev_t * locdev;
    locdev = container_of(tm, struct local_dev_t, timer);

    /* 获取按键状态 */
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
    
    if(stat == KEY_KEEP)
        return;

    /* 只在按键按下和释放时上报input事件 */
    input_report_key(locdev->inputdev, BTN_1, locdev->key_stat);
	input_sync(locdev->inputdev);
}

static int hardware_init(struct local_dev_t *dev)
{ 
    int ret = 0;
    /* 设置gpio状态 */
    /* 设置定时器 */
    /* 设置中断服务函数 */
    return ret;
}

/* 匹配成功后调用 */
int input_probe(struct platform_device *pdev)
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
    /* 从platform设备中获取设备节点 */
    local_dev->nd = pdev->dev.of_node;
    timer_setup(&local_dev->timer, timer_callback, 0);
    /* 根据设备节点初始化硬件 */
    ret = hardware_init(local_dev);
    if (ret)
        goto free_localdev;

    /* Start init input-device */
    inputdev = input_allocate_device();
	if (!inputdev) {
		ret = -ENOMEM;
		printk(KERN_ERR "%s: Not enough memory.\n", __func__);
		goto free_hardware;
	}
    /* 设置要上报的事件 */
    set_bit(EV_KEY, inputdev->evbit);
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
    input_free_device(inputdev);
free_hardware:
    /* free_hardware */
free_localdev:
    devm_kfree(&pdev->dev, local_dev);
out:
    return ret;
}
/* 驱动或设备一方卸载后调用 */
int input_remove(struct platform_device *pdev)
{
    struct local_dev_t *localdev = pdev->dev.driver_data;

    input_unregister_device(localdev->inputdev);
    return 0;
}

static const struct of_device_id local_match[] = {
	{ .compatible   = "eden,key" },   /* 根据此属性和设备树上的compatible进行匹配*/
	{},
};
MODULE_DEVICE_TABLE(of, local_match);
struct platform_driver input_driver = {
    .driver = {
		.name		    = "input_driver",   /* display in /sys/bus/platform/drivers */            
		.of_match_table = of_match_ptr(local_match),
	},
    .probe = input_probe,
    .remove = input_remove,
};

/* 等同于在init和exit中仅仅注册注销驱动 */
module_platform_driver(input_driver);

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("input_device module"); // 描述
MODULE_ALIAS("Input Learn");               // 别名
