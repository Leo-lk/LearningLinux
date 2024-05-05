#include <linux/platform_device.h>
#include <linux/of.h>
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

MODULE_LICENSE("GPL");

/*
// In RV1126, rv1126-alientek.dts

/{
    gpioled {
        compatible = "gpioleds";
        pinctrl-names = "eden,led0";
        pinctrl-0 = <&led0_pins>;
        gpios = <&gpio3 RK_PD4 GPIO_ACTIVE_HIGH>;
        status = "okay";
    };
}

&pinctrl {
    ledctl {
        /omit-if-no-ref/
        led0_pins:led0-pin{
            rockchip,pins =
            // auddsm_ln 
            <3 RK_PD4 RK_FUNC_GPIO &pcfg_pull_none>;
        };
    };
}
*/

#define LEDOFF 				        0			/* 关灯 */
#define LEDON 				        1			/* 开灯 */
#define DEVICE_NUM                  (1)

struct local_dev_t {
    struct cdev cdev;
    struct class * cls;
    struct device * dev;
    dev_t major;
    dev_t minor;
    struct device_node	*nd;
    unsigned int led_gpio;
};

static struct local_dev_t * local_dev;
static struct class * cls;
static dev_t major;

static int hello_open(struct inode *inode, struct file *filep)
{
    struct local_dev_t * locdev;
    printk("kk open\n");
    /* 通过成员变量查找返回含有该成员变量的结构体，inode->i_cdev指向一个globalmem_dev类型的cdev成员变量*/
    locdev = container_of(inode->i_cdev, struct local_dev_t, cdev);
    filep->private_data = locdev;
    return 0;
}
static int hello_release(struct inode *inode, struct file *filep)
{
    printk("kk release\n");
    
    return 0;
}

static ssize_t hello_write(struct file *filep, const char __user *buf, size_t size, loff_t *pos)
{
    int ret;
    unsigned char inbuf[0x10];
	unsigned char ledstat;
	struct local_dev_t *dev = filep->private_data;

    if (size > 0x10) {
        printk("kernel write length over: %d\r\n", size);
        return EFAULT;
    }

	ret = copy_from_user(inbuf, buf, size); /* 接收APP发送过来的数据 */
	if(ret < 0) {
		printk("kernel write failed!\r\n");
		return -EFAULT;
	}
    printk("input val:%d, size:%d\r\n", inbuf[0], size);

    ledstat = inbuf[0]<'0'? inbuf[0] : inbuf[0] - '0';

	if(ledstat == LEDON) {	
		gpio_set_value(dev->led_gpio, 1);	/* 打开LED灯 */
	} else if(ledstat == LEDOFF) {
		gpio_set_value(dev->led_gpio, 0);	/* 关闭LED灯 */
	}
    else{
        printk("another value: %d\r\n", ledstat);
    }
	return size;
}
static struct file_operations hello_ops = {
    .open       = hello_open,
    .release    = hello_release,
    .write      = hello_write,
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

	/* 1、 获取设备树中的gpio属性，得到LED所使用的LED编号 */
	dev->led_gpio = of_get_named_gpio(dev->nd, "gpios", 0);
	if(dev->led_gpio < 0) {
		printk("can't get gpios");
		return -EINVAL;
	}
	printk("gpios num = %d\r\n", dev->led_gpio);

	/* 2.向gpio子系统申请使用GPIO */
	ret = gpio_request(dev->led_gpio, "LED-GPIO");
    if (ret) {
        pr_err( "gpioled: Failed to request gpios\n");
        return ret;
	}

	/* 3、设置GPIO为输出，并且输出低电平，默认关闭LED灯 */
	ret = gpio_direction_output(dev->led_gpio, 0);
	if(ret < 0) {
		printk("can't set gpio!\r\n");
	}

    return 0;
}

/* 匹配(match)成功后调用 */
int local_device_probe(struct platform_device *pdev)
{
    int ret = 0, i;
    dev_t devno;

    printk("driver match ok \n");
    local_dev = kzalloc(sizeof(struct local_dev_t)*DEVICE_NUM, GFP_KERNEL);
    if (!local_dev) {
        ret = -ENOMEM;
        return ret;
    }
    /* 从平台设备中获取node，来初始化引脚 */
    local_dev->nd = pdev->dev.of_node;
    ret = get_gpio_msg(local_dev);
    if (ret){
        goto out_err_chr_region;
    }

    /* 下面可以注册cdev向用户提供交互接口 */
    {
        devno = MKDEV(major, 0);

        if(major){
            /* 根据devno注册设备号 */
            ret = register_chrdev_region(devno, DEVICE_NUM, "globalmem");
        }
        else{
            /* 自动分配主设备号并注册 */
            ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalmem");
            major = MAJOR(devno);
        }
        if (ret < 0){
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
    }

    /* 把local_dev放到pdev->dev.drvdata中 */
    platform_set_drvdata(pdev, &local_dev);

    return 0;

out_err_gmem_dev:
    class_destroy(cls);
out_err_cls_crt:
    unregister_chrdev_region(major, DEVICE_NUM);
out_err_chr_region:
    kfree(local_dev);

    return ret;
}
/* 驱动或设备一方卸载(exit)后调用 */
int local_remove(struct platform_device *pdev)
{
    int i = 0;
    dev_t devno;

    printk("driver local remove \n");
    
    for (i = 0; i < DEVICE_NUM; i++) {
        devno = MKDEV(major, i);
        device_destroy(cls, devno);
        cdev_del(&(local_dev + i)->cdev);
    }
    
    class_destroy(cls);
    unregister_chrdev_region(major, DEVICE_NUM);
    kfree(local_dev);
    gpio_free(local_dev->led_gpio); 
    return 0;
}

static const struct of_device_id local_match[] = {
	{ .compatible   = "gpioleds" },   /* 根据此属性和设备树上的compatible进行匹配*/
	{},
};
MODULE_DEVICE_TABLE(of, local_match);

static struct platform_driver local_driver = {
	.driver	= {
		.name		    = "gpioleds_dri",   /* display in /sys/bus/platform/drivers */            
		.of_match_table = of_match_ptr(local_match),
	},
	.probe	= local_device_probe,
    .remove = local_remove,
};


#if 0
static int hello_init(void)
{
    printk("driver Hello_init \n");
    
    /* 注册 */
    return platform_driver_register(&hello_driver);
}
static void hello_exit(void)
{
    printk("driver Hello_exit \n");
    /* 卸载 */
    platform_driver_unregister(&hello_driver);
    return;
}
module_init(hello_init);
module_exit(hello_exit);
#else
/* 等同于在init和exit中仅仅注册注销驱动 */
module_platform_driver(local_driver);
#endif

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");   // 作者
MODULE_DESCRIPTION("register_chrdev test");     // 描述
MODULE_ALIAS("Driver Learn");                   // 别名
