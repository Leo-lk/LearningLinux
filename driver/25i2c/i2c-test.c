/*
    i2c-dev.c - i2c-bus driver, char device interface

    Copyright (C) 1995-97 Simon G. Vogl
    Copyright (C) 1998-99 Frodo Looijaard <frodol@dds.nl>
    Copyright (C) 2003 Greg Kroah-Hartman <greg@kroah.com>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/

/* Note that this is a complete rewrite of Simon Vogl's i2c-dev module.
   But I have used so much of his original code and ideas that it seems
   only fair to recognize him as co-author -- Frodo */

/* The I2C_RDWR ioctl code is written by Kolja Waschk <waschk@telos.de> */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/compat.h>

struct pcf8563 {
	struct device *dev;
	struct cdev cdev;
	struct i2c_client *client;
};
static struct class * cls;
dev_t major = 0;
dev_t minor = 0;

static int pcf8563dev_open(struct inode *inode, struct file *filep)
{
    struct pcf8563 * locdev;
    /* 通过成员变量查找返回含有该成员变量的结构体，inode->i_cdev指向一个globalmem_dev类型的cdev成员变量*/
    locdev = container_of(inode->i_cdev, struct pcf8563, cdev);
    filep->private_data = locdev;
    printk("kk open, slave addr is %d\n", locdev->client->addr);
    return 0;
}

static int pcf8563dev_release(struct inode *inode, struct file *filep)
{
    printk("kk release\n");
    
    return 0;
}

static ssize_t pcf8563dev_read(struct file *filep, char __user *buf, size_t size, loff_t *pos)
{
    char *tmp;
	int ret;

	struct pcf8563 * locdev = filep->private_data;

	if (size > 8192)
		size = 8192;

	tmp = kmalloc(size, GFP_KERNEL);
	if (tmp == NULL)
		return -ENOMEM;

	printk("i2c-dev: i2c-%d reading %zu bytes.\n",
		iminor(file_inode(filep)), size);

	ret = i2c_master_recv(locdev->client, tmp, size);
	if (ret >= 0)
		ret = copy_to_user(buf, tmp, size) ? -EFAULT : ret;
	kfree(tmp);
	return ret;
}

static ssize_t pcf8563dev_write(struct file *filep, const char __user *buf, size_t size, loff_t *pos)
{
    int ret;
	char *tmp;
	struct pcf8563 * locdev = filep->private_data;

	if (size > 8192)
		size = 8192;

	tmp = memdup_user(buf, size);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	printk("i2c-dev: i2c-%d writing %zu bytes.\n",
		iminor(file_inode(filep)), size);
	
	ret = i2c_master_send(locdev->client, tmp, size);
	kfree(tmp);
	return ret;
}

static const struct file_operations pcfdev_fops = {
	.owner		= THIS_MODULE,
	.read		= pcf8563dev_read,
	.write		= pcf8563dev_write,
	.open		= pcf8563dev_open,
	.release	= pcf8563dev_release,
};

static int local_cdev_register(struct pcf8563 *pcfdev)
{
    int ret = 0;
    dev_t devno;

	devno = MKDEV(major, 0);
    if(major){
        /* 根据devno注册设备号 */
        ret = register_chrdev_region(devno, 1, "pcf8563_s");
    }
    else{
        /* 自动分配主设备号并注册 */
        ret = alloc_chrdev_region(&devno, 0, 1, "pcf8563_s");
        major = MAJOR(devno);
		minor = MINOR(devno);
    }
    if (ret < 0){
        printk("register_chrdev fail, ret = %d \n", ret);
        goto out;
    }
	cls = class_create(THIS_MODULE, "pcf8563_s");
	if (IS_ERR(cls)) {
		ret = PTR_ERR(cls);
		goto out_unchr;
	}

    cdev_init(&pcfdev->cdev, &pcfdev_fops);
    ret = cdev_add(&pcfdev->cdev, devno, 1);
    if(ret < 0){
        printk("cdev add fail %d\n", ret);
        ret = PTR_ERR(pcfdev);
        goto out_cls_dest;
    }

    /* 在/sys/class/pcf8563_s创建pcf8563dev设备 */
    pcfdev->dev = device_create(cls, &pcfdev->client->adapter->dev, devno, NULL, "pcf8563dev");
    if(IS_ERR (pcfdev->dev)){
        printk("device_create failed\n");
        ret = PTR_ERR(pcfdev);
        goto out_cdev_del;
    }
	return 0;

out_cdev_del:
	cdev_del(&pcfdev->cdev);
out_cls_dest:
    class_destroy(cls);
out_unchr:
    unregister_chrdev_region(major, 1);
out:
    return ret;
}

static int pcf8563_i2c_probe(struct i2c_client *client,
				const struct i2c_device_id *id)
{
	int ret;
	struct pcf8563 *pcf8563;

	dev_dbg(&client->dev, "%s\n", __func__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
		return -ENODEV;
	/* 该函数申请的内存在设备注销时会自动free */
	pcf8563 = devm_kzalloc(&client->dev, sizeof(struct pcf8563),
				GFP_KERNEL);
	if (!pcf8563)
		return -ENOMEM;
	pcf8563->client = client;
	i2c_set_clientdata(client, pcf8563);
	/* 创建设备节点 */
	ret = local_cdev_register(pcf8563);
	if (ret) {
		goto out;
	}

    printk("kk i2c probe, slave addr is %d\n", pcf8563->client->addr);

	return 0;

out :
	return ret;
}
static int pcf8563_i2c_remove(struct i2c_client *client)
{
    dev_t devno;
	struct pcf8563 *pcf8563 = i2c_get_clientdata(client);
	devno = MKDEV(major, 0);

    printk("pcf8563_i2c_remove \n");
    
	device_destroy(cls, devno);
	cdev_del(&pcf8563->cdev);
    class_destroy(cls);
    unregister_chrdev_region(major, 1);
	return 0;
}

static const struct of_device_id pcf8563_dt_ids[] = {
	{.compatible = "edenrtc,pcf8563",},
	{}
};
MODULE_DEVICE_TABLE(of, pcf8563_dt_ids);

static const struct i2c_device_id id_table[] = {
	{"pcf8563", 0 },
	{ }
};

static struct i2c_driver pcf8563_i2c_driver  = {
	.driver = {
		.name  = "eden-pcf8563",
		.owner = THIS_MODULE,
		// .suspend        = &pcf8563_core_suspend,
		// .retume         = &pcf8563_core_retume,
		.of_match_table = of_match_ptr(pcf8563_dt_ids),
	},
	.probe		= &pcf8563_i2c_probe,
	.remove		= &pcf8563_i2c_remove,
	.id_table	= id_table,
};

module_i2c_driver(pcf8563_i2c_driver);

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");
MODULE_DESCRIPTION("rtc pcf8563 driver, and dev in /dev/pcf8563dev");
MODULE_LICENSE("GPL");
