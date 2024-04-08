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
#include <linux/interrupt.h>
#include <linux/of_irq.h>

MODULE_LICENSE("GPL");

#define DEVICE_NUM                  (1)
#define TIME_TRIG                   (15)
#define CHRDEV_NAME                 "key-irq"

struct local_dev_t {
    struct cdev cdev;
    struct class * cls;
    struct device * dev;
    dev_t major;
    dev_t minor;
    struct device_node	*nd;
    unsigned int key_gpio;
    struct timer_list timer;
    struct work_struct key_workque;
    spinlock_t lock;                /* protect key_stat*/
    int key_stat;
    int irq;
};

enum key_stat{
    KEY_PRESS = 0,
    KEY_RELEASE,
    KEY_KEEP
};

static struct local_dev_t * local_dev;
static struct class * cls;
static dev_t major;

static int get_gpio_msg(struct local_dev_t *dev);
static int key_set_irq(struct local_dev_t *dev);

void key_workque_func(struct work_struct *work)
{
    struct local_dev_t * locdev;
    locdev = container_of(work, struct local_dev_t, key_workque);
    mod_timer(&locdev->timer, jiffies + msecs_to_jiffies(TIME_TRIG));
}

irqreturn_t key_handle(int irq, void *dev_id)
{
    struct local_dev_t * locdev = dev_id;
    schedule_work(&locdev->key_workque);
    return IRQ_HANDLED;
}

static void callback(struct timer_list * tm)
{
    static int cur_stat = 0, last_stat = 0, stat = KEY_KEEP;
    unsigned long flags;
    struct local_dev_t * locdev;
    locdev = container_of(tm, struct local_dev_t, timer);

    /* 按键消抖 */
    cur_stat = gpio_get_value(locdev->key_gpio);
    if (cur_stat != last_stat) {
        stat = (cur_stat == 0)?KEY_RELEASE:KEY_PRESS;
    }
    else{
        stat = KEY_KEEP;
    }
    last_stat = cur_stat;
    /* Update key status */
    spin_lock_irqsave(&locdev->lock, flags);
    locdev->key_stat = stat;
    spin_unlock_irqrestore(&locdev->lock, flags);
}

/*  inode 存放不变的信息；
    file 存放可变的信息
    每打开一次文件都会创建一个file
*/
static int hello_open(struct inode *inode, struct file *filep)
{
    int ret;
    struct local_dev_t * locdev;
    printk("kk open\n");
    /* 通过成员变量查找返回含有该成员变量的结构体，inode->i_cdev指向一个globalmem_dev类型的cdev成员变量*/
    locdev = container_of(inode->i_cdev, struct local_dev_t, cdev);
    filep->private_data = locdev;

    ret = get_gpio_msg(locdev);
    if (ret)
        return ret;
    timer_setup(&locdev->timer, callback, 0);
    ret = key_set_irq(locdev);
    return ret;
}

static int hello_release(struct inode *inode, struct file *filep)
{
    struct local_dev_t * locdev;
    locdev = filep->private_data;
    printk("kk release\n");

    gpio_free(locdev->key_gpio); /* 释放GPIO */
    del_timer_sync(&locdev->timer);
    flush_work(&locdev->key_workque);
    free_irq(locdev->irq, locdev);
    return 0;
}

static ssize_t hello_read(struct file *filep, char __user *buf, size_t size, loff_t *pos)
{
    int stat;
	unsigned long flags;
	struct local_dev_t *locdev = filep->private_data;

    spin_lock_irqsave(&locdev->lock, flags);
    stat = locdev->key_stat;
    spin_unlock_irqrestore(&locdev->lock, flags);

    printk("key val:%d\r\n", stat);
    return size;
}

static struct file_operations hello_ops = {
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
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
    INIT_WORK(&dev->key_workque, key_workque_func);

    ret = request_irq(dev->irq, key_handle, irq_type, "key-irq", dev);
    if (ret) {
		printk("irq: Failed to request_irq, ret=%d\n", ret);
    }
    return ret;
}

static int get_gpio_msg(struct local_dev_t *dev)
{
    int ret = 0;
	const char *str;

    /* 设置LED所使用的GPIO */
	/* 1、获取设备节点：key */
	dev->nd = of_find_node_by_path("/key");
	if(dev->nd == NULL) {
		printk("key node not find!\r\n");
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

    if (strcmp(str, "eden,key")) {
        printk("gpioled: Compatible match failed\n");
        return -EINVAL;
    }

	/* 4、 获取设备树中的gpio属性，得到LED所使用的LED编号 */
	dev->key_gpio = of_get_named_gpio(dev->nd, "keys-gpios", 0);
	if(dev->key_gpio < 0) {
		printk("can't get gpios");
		return -EINVAL;
	}
	printk("gpios num = %d\r\n", dev->key_gpio);

	/* 5.向gpio子系统申请使用GPIO */
	ret = gpio_request(dev->key_gpio, "LED-GPIO");
    if (ret) {
        printk(KERN_ERR "gpioled: Failed to request gpios\n");
        return ret;
	}

	/* 6、设置GPIO为输入 */
	ret = gpio_direction_input(dev->key_gpio);
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
        ret = register_chrdev_region(devno, DEVICE_NUM, CHRDEV_NAME);
    }
    else
    {
        /* 自动分配主设备号并注册 */
        ret = alloc_chrdev_region(&devno, 0, DEVICE_NUM, CHRDEV_NAME);
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

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
