#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/wait.h> 
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_gpio.h>
#include <asm/mach/map.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/time.h>
#include <linux/timer.h>
#include "beep.h"

MODULE_LICENSE("GPL");

#define DEVICE_NUM                  (1)
#define TIME_TRIG                   (2000)

struct local_dev_t {
    struct cdev cdev;
    struct class * cls;
    struct device * dev;
    dev_t major;
    dev_t minor;
    struct device_node	*nd;
    unsigned int led_gpio;
    unsigned long tim_expires;
    struct timer_list timer;
    spinlock_t lock;            /* protect tim_expires*/
};

static struct local_dev_t * local_dev;
static struct class * cls;
static dev_t major;

static int get_gpio_msg(struct local_dev_t *dev);
static void callback(struct timer_list * tm);

/*  inode 存放不变的信息；
    file 存放可变的信息
    每打开一次文件都会创建一个file
*/
static int hello_open(struct inode *inode, struct file *filep)
{
    int ret;
    struct local_dev_t * locdev;
	unsigned long flags;
    printk("kk open\n");
    /* 通过成员变量查找返回含有该成员变量的结构体，inode->i_cdev指向一个globalmem_dev类型的cdev成员变量*/
    locdev = container_of(inode->i_cdev, struct local_dev_t, cdev);
    filep->private_data = locdev;

    ret = get_gpio_msg(locdev);
    if (ret)
    {
        return ret;
    }
    timer_setup(&locdev->timer, callback, 0);
    /* Set tim_expires */
    spin_lock_irqsave(&locdev->lock, flags);
    locdev->tim_expires = TIME_TRIG;
    spin_unlock_irqrestore(&locdev->lock, flags);

    return 0;
}

static int hello_release(struct inode *inode, struct file *filep)
{
    struct local_dev_t * locdev;
    locdev = filep->private_data;
    printk("kk release\n");
    gpio_free(locdev->led_gpio); /* 释放GPIO */
    del_timer_sync(&locdev->timer);
    return 0;
}

static void callback(struct timer_list * tm)
{
    static int stat = 0;
    unsigned long flags, tim_expires = 0;
    struct local_dev_t * locdev;
    locdev = container_of(tm, struct local_dev_t, timer);
    printk("call back\n");

    stat = stat==1?0:1;
    /* Overturn led */
    gpio_set_value(locdev->led_gpio, stat);

    /* Read tim_expires */
    spin_lock_irqsave(&locdev->lock, flags);
    tim_expires = locdev->tim_expires;
    spin_unlock_irqrestore(&locdev->lock, flags);

    printk("call back: modify, %lu\n", tim_expires);
    /* Restart timer */
    mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(tim_expires));

    //add_timer(&locdev->timer);
}

long hello_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    long ret = 0;
    unsigned long flags, tim_expires = 0;
    struct local_dev_t * locdev;
    locdev = filep->private_data;

    // void __user *argp = (void __user *)arg;
    // int __user *p = argp;

    if (_IOC_TYPE(cmd) != DEV_FIFO_TYPE)
    {
        pr_err("cmd   %u,bad magic 0x%x/0x%x.\n", cmd, _IOC_TYPE(cmd), DEV_FIFO_TYPE);
        return -ENOTTY;
    }
    if (_IOC_DIR(cmd) & _IOC_WRITE)
        ret = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));

    if (ret)
    {
        pr_err("bad   access %ld.\n", ret);
        return -EFAULT;
    }

    switch (cmd)
    {
    case DEV_CLOSE_TIMER:
        printk("DEV_CLOSE_TIMER\n");
        del_timer_sync(&locdev->timer);
        break;
    case DEV_MODIFY_TIMER:
        printk("DEV_MODIFY_TIMER \n");
        /* Write tim_expires */
        spin_lock_irqsave(&locdev->lock, flags);
        locdev->tim_expires = arg;
        spin_unlock_irqrestore(&locdev->lock, flags);

        mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(arg));
        break;
    case DEV_OPEN_TIMER:
        printk("DEV_OPEN_TIMER \n");
        /* Read tim_expires */
        spin_lock_irqsave(&locdev->lock, flags);
        tim_expires = locdev->tim_expires;
        spin_unlock_irqrestore(&locdev->lock, flags);

        mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(tim_expires));
        // add_timer(&locdev->timer);
        break;
    default:
        return -EINVAL;
    }
    return ret;
}

static struct file_operations hello_ops = {
    .open = hello_open,
    .release = hello_release,
    .unlocked_ioctl = hello_ioctl,
};

static int local_cdev_init(struct local_dev_t *dev, struct class * cls, dev_t major, int index)
{
    int ret = 0;
    dev_t devno = MKDEV(major, index);
    dev->cls = cls;
    dev->minor = index;
    dev->major = major;

    cdev_init(&dev->cdev, &hello_ops);
    ret = cdev_add(&dev->cdev, devno, 1);
    if(ret < 0){
        printk("cdev add fail %d\n", ret);
        ret = PTR_ERR(dev);
        return ret;
    }

    /* 在/sys/class/hellocls创建hellodevice设备 */
    dev->dev = device_create(dev->cls, NULL, devno, NULL, "localdevice%d", index);
    if(IS_ERR (dev->dev)){
        printk("device_create failed\n");
        ret = PTR_ERR(dev);
        return ret;
    }

    return 0;
}
static int get_gpio_msg(struct local_dev_t *dev)
{
    int ret = 0;
	const char *str;

    /* 设置LED所使用的GPIO */
	/* 1、获取设备节点：gpioled */
	dev->nd = of_find_node_by_path("/gpioled");
	if(dev->nd == NULL) {
		printk("gpioled node not find!\r\n");
		return -EINVAL;
	}

	/* 2.读取status属性 */
	ret = of_property_read_string(dev->nd, "status", &str);
	if(ret < 0) 
	    return -EINVAL;

	if (strcmp(str, "okay"))
        return -EINVAL;
    
	/* 3、获取compatible属性值并进行匹配 */
	ret = of_property_read_string(dev->nd, "compatible", &str);
	if(ret < 0) {
		printk("gpioled: Failed to get compatible property\n");
		return -EINVAL;
	}

    if (strcmp(str, "gpioleds")) {
        printk("gpioled: Compatible match failed\n");
        return -EINVAL;
    }

	/* 4、 获取设备树中的gpio属性，得到LED所使用的LED编号 */
	dev->led_gpio = of_get_named_gpio(dev->nd, "gpios", 0);
	if(dev->led_gpio < 0) {
		printk("can't get gpios");
		return -EINVAL;
	}
	printk("gpios num = %d\r\n", dev->led_gpio);

	/* 5.向gpio子系统申请使用GPIO */
	ret = gpio_request(dev->led_gpio, "LED-GPIO");
    if (ret) {
        printk(KERN_ERR "gpioled: Failed to request gpios\n");
        return ret;
	}

	/* 6、设置GPIO为输出，并且输出低电平，默认关闭LED灯 */
	ret = gpio_direction_output(dev->led_gpio, 0);
	if(ret < 0) {
		printk("can't set gpio!\r\n");
	}

    return 0;
}

static int hello_init(void)
{
    int ret = 0, i;
    dev_t devno;
    
    printk("Hello_init \n");

    local_dev = kzalloc(sizeof(struct local_dev_t)*DEVICE_NUM, GFP_KERNEL);
    if (!local_dev) {
        ret = -ENOMEM;
        return ret;
    }

    devno = MKDEV(major, 0);

    if(major)
    {
        /* 根据devno注册设备号 */
        ret = register_chrdev_region(devno, DEVICE_NUM, "globalmem");
    }
    else
    {
        /* 自动分配主设备号并注册 */
        ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalmem");
        major = MAJOR(devno);
    }
    if (ret < 0)
    {
        printk("register_chrdev fail, ret = %d \n", ret);
        goto out_err_chr_region;
    }

    cls = class_create(THIS_MODULE, "hellocls");
    if(IS_ERR (cls)){
        printk("class_create failed\n");
        ret = PTR_ERR(cls);
        goto out_err_cls_crt;
    }

    for (i = 0; i < DEVICE_NUM; i++) {
        ret = local_cdev_init(local_dev+i, cls, major, i);
        if (ret) {
            goto out_err_gmem_dev;
        }
    }

    return 0;

out_err_gmem_dev:
    class_destroy(cls);
out_err_cls_crt:
    unregister_chrdev_region(major, DEVICE_NUM);
out_err_chr_region:
    kfree(local_dev);

    return ret;
}

static void hello_exit(void)
{
    int i = 0;
    dev_t devno;
    printk("Hello_exit \n");
    for (i = 0; i < DEVICE_NUM; i++) {
        devno = MKDEV(major, i);
        device_destroy(cls, devno);
        cdev_del(&(local_dev + i)->cdev);
    }
    
    class_destroy(cls);
    unregister_chrdev_region(major, DEVICE_NUM);
    kfree(local_dev);
    return;
}

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>"); // 作者
MODULE_DESCRIPTION("Timer and Led"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
