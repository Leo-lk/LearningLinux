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
#include <linux/slab.h>

#include "beep.h"

MODULE_LICENSE("GPL");

#define MEMBUF_SIZE                 (1024)
#define DEVICE_NUM                  (10)

static dev_t devno;
struct class * cls;
struct device * class_dev;

static struct globalmem_dev {
    struct cdev cdev;
    unsigned char membuf[MEMBUF_SIZE];
};

static struct globalmem_dev * gmem_dev;

static int major = 0;

/*  inode 存放不变的信息；
    file 存放可变的信息
    每打开一次文件都会创建一个file
*/
static int hello_open(struct inode *inode, struct file *filep)
{
    struct globalmem_dev * dev;
    printk("kk hello open\n");
    /* 通过成员变量查找返回含有该成员变量的结构体，inode->i_cdev指向一个globalmem_dev类型的cdev成员变量*/
    dev = container_of(inode->i_cdev, struct globalmem_dev, cdev);
    filep->private_data = dev;
    return 0;
}

static int hello_release(struct inode *inode, struct file *filep)
{
    printk("kk hello release\n");
    
    return 0;
}

static ssize_t hello_read(struct file *filep, char __user *buf, size_t size, loff_t *pos)
{
    int ret = 0;
    unsigned long p = *pos;
    unsigned int cnt = size;

    struct globalmem_dev *gmem_dev = filep->private_data;

    if (p > MEMBUF_SIZE)
        return 0;
    if (cnt > MEMBUF_SIZE - p) {
        cnt = MEMBUF_SIZE - p;
    }

    if (copy_to_user(buf, gmem_dev->membuf, cnt))
    {
        ret = -EFAULT;
    }
    else {
        *pos += cnt;
        ret = cnt;
    }

    return ret;
}

static ssize_t hello_write(struct file *filep, const char __user *buf, size_t size, loff_t *pos)
{
    int ret = 0;
    unsigned long p = *pos;
    unsigned int cnt = size;

    struct globalmem_dev *gmem_dev = filep->private_data;

    if (p > MEMBUF_SIZE)
        return 0;
    if (cnt > MEMBUF_SIZE - p) {
        cnt = MEMBUF_SIZE - p;
    }
    
    if (copy_from_user(gmem_dev->membuf, buf, cnt)){
        ret = -EFAULT;
    }
    else {
        *pos += cnt;
        ret = cnt;
        printk("hello_write kbuf: %s\n", gmem_dev->membuf);
    }

    return ret;
}

static loff_t hello_lseek(struct file *filep, loff_t offset, int whence)
{
    loff_t ret = 0;
    switch (whence) {
    case SEEK_SET:
        if ((offset < 0) || (offset > MEMBUF_SIZE)) {
            ret = -EINVAL;
            break;
        }
        filep->f_pos = offset;
        ret = filep->f_pos;
        printk("hello_lseek SEEK_SET kbuf: %lld\n", filep->f_pos);
        break;
    case SEEK_CUR:
        if ((filep->f_pos + offset) > MEMBUF_SIZE) {
            ret = -EINVAL;
            break;
        }
        if ((filep->f_pos + offset) < 0) {
            ret = -EINVAL;
            break;
        }
        filep->f_pos += offset;
        ret = filep->f_pos;
        printk("hello_lseek SEEK_CUR kbuf: %lld\n", filep->f_pos);
    default:
        ret = -EINVAL;
        break;
    }
    return ret;
}

static int knum = 99;

long hello_ioctl(struct file *filep, unsigned int cmd, unsigned long arg)
{
    long err = 0, ret = 0;

    void __user *argp = (void __user *)arg;
    int __user *p = argp;

    if (_IOC_TYPE(cmd) != DEV_FIFO_TYPE)
    {
        pr_err("cmd   %u,bad magic 0x%x/0x%x.\n", cmd, _IOC_TYPE(cmd), DEV_FIFO_TYPE);
        return -ENOTTY;
    }
    if (_IOC_DIR(cmd) & _IOC_READ)
        ret = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
    else if (_IOC_DIR(cmd) & _IOC_WRITE)
        ret = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));

    if (ret)
    {
        pr_err("bad   access %ld.\n", ret);
        return -EFAULT;
    }

    switch (cmd)
    {
    case DEV_FIFO_CLEAN:
        printk("DEV_FIFO_CLEAN\n");
        break;
    case DEV_FIFO_GETVALUE:
        err = put_user(knum, p);
        printk("DEV_FIFO_GETVALUE %d\n", knum);
        break;
    case DEV_FIFO_SETVALUE:
        err = get_user(knum, p);
        printk("DEV_FIFO_SETVALUE %d\n", knum);
        break;
    default:
        return -EINVAL;
    }
    return err;
}

static struct file_operations hello_ops = {
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .write = hello_write,
    .unlocked_ioctl = hello_ioctl,
    .llseek = hello_lseek,
};

static int gmem_cdev_init(struct globalmem_dev *dev, int index)
{
    int ret = 0;
    dev_t devno = MKDEV(major, index);
    cdev_init(&dev->cdev, &hello_ops);
    ret = cdev_add(&dev->cdev, devno, 1);
    if(ret < 0){
        printk("cdev add fail %d\n", ret);
    }
    return ret;
}

static int gmem_class_init(struct device *class_dev, int index)
{
    int ret = 0;
    dev_t devno = MKDEV(major, index);
    /* 在/sys/class/hellocls创建hellodevice设备 */
    class_dev = device_create(cls, NULL, devno, NULL, "hellodevice%d", index);
    if(IS_ERR (class_dev)){
        printk("device_create failed\n");
        ret = PTR_ERR(class_dev);
    }
    return ret;
}

static int hello_init(void)
{
    int res, i;
    devno = MKDEV(major, 0);
    printk("Hello_init \n");

    if(major)
    {
        res = register_chrdev_region(devno, DEVICE_NUM, "globalmem");
    }
    else
    {
        res = alloc_chrdev_region(&devno, 0, DEVICE_NUM, "globalmem");
        major = MAJOR(devno);
    }
    if (res < 0)
    {
        printk("register_chrdev fail, res = %d \n", res);
        return res;
    }

    gmem_dev = kzalloc(sizeof(struct globalmem_dev)*DEVICE_NUM, GFP_KERNEL);
    if (!gmem_dev) {
        res = -ENOMEM;
        goto out_err_1;
    }
    class_dev = kzalloc(sizeof(struct device)*DEVICE_NUM, GFP_KERNEL);
    if (!class_dev) {
        res = -ENOMEM;
        goto out_err_2;
    }

    cls = class_create(THIS_MODULE, "hellocls");
    if(IS_ERR (cls)){
        printk("class_create failed\n");
        res = PTR_ERR(cls);
        goto out_err_3;
    }

    for (i = 0; i < DEVICE_NUM; i++) {
        gmem_cdev_init(gmem_dev+i, i);
        gmem_class_init(class_dev+i, i);
    }

    return 0;
out_err_3:
    kfree(class_dev);
out_err_2:
    kfree(gmem_dev);
out_err_1:
    unregister_chrdev_region(major, DEVICE_NUM);
    return res;
}

static void hello_exit(void)
{
    int i = 0;
    dev_t devno;
    printk("Hello_exit \n");
    for (i = 0; i < DEVICE_NUM; i++) {
        devno = MKDEV(major, i);
        device_destroy(cls, devno);
        cdev_del(&(gmem_dev + i)->cdev);
    }
    
    class_destroy(cls);
    kfree(class_dev);
    kfree(gmem_dev);
    unregister_chrdev_region(major, DEVICE_NUM);
    return;
}

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
