#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/device.h>
#include "beep.h"

MODULE_LICENSE("GPL");

static int major = 200;
static int minor = 0;
static dev_t devno;
struct class *cls;
struct device *class_dev;
#define KMAXLEN 32
static char test_buf[KMAXLEN + 1] = "kernel test -----";

static struct semaphore test_sem;


/*  inode 存放不变的信息；
    file 存放可变的信息
    每打开一次文件都会创建一个file
*/
static int hello_open(struct inode *inode, struct file *filep)
{
    printk("kk hello open start\n");
    /* 和down()相比，down_interruptible的休眠可被用户发送的信号中断, 而down()只会一直等待信号量可用 */
    if(down_interruptible(&test_sem))
        return -ERESTARTSYS;

    printk("kk hello open finish\n");
    return 0;
}

static int hello_release(struct inode *inode, struct file *filep)
{
    printk("kk hello release\n");
    up(&test_sem);
    return 0;
}

static ssize_t hello_read(struct file *filep, char __user *buf, size_t size, loff_t *pos)
{
    int error;

    if (size > strlen(test_buf))
        size = strlen(test_buf);
    if (copy_to_user(buf, test_buf, size))
    {
        error = -EFAULT;
        return error;
    }
    return size;
}

static ssize_t hello_write(struct file *filep, const char __user *buf, size_t size, loff_t *pos)
{
    int error;

    if (size > KMAXLEN)
        size = KMAXLEN;
    memset(test_buf, 0, sizeof(test_buf));
    if (copy_from_user(test_buf, buf, size))
    {
        error = -EFAULT;
        return error;
    }
    printk("hello_write kbuf: %s\n", test_buf);
    return 0;
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
};

static int hello_init(void)
{
    int res;
    printk("Hello_init \n");
    res = register_chrdev(major, "kk dev", &hello_ops);
    if (res < 0)
    {
        printk("register_chrdev fail, res = %d \n", res);
        return res;
    }

    cls = class_create(THIS_MODULE, "hellocls");
    if (IS_ERR(cls))
    {
        printk("class_create failed\n");
        res = PTR_ERR(cls);
        goto out_err_1;
    }
    devno = MKDEV(major, minor);
    /* 在/sys/class/hellocls创建hellodevice设备 */
    class_dev = device_create(cls, NULL, devno, NULL, "hellodevice");
    if (IS_ERR(class_dev))
    {
        printk("device_create failed\n");
        res = PTR_ERR(class_dev);
        goto out_err_2;
    }

    sema_init(&test_sem, 1);

    return 0;
out_err_2:
    class_destroy(cls);

out_err_1:
    unregister_chrdev(major, "kk dev");
    return res;
}

static void hello_exit(void)
{
    printk("Hello_exit \n");
    device_destroy(cls, devno);
    class_destroy(cls);
    unregister_chrdev(major, "kk dev");
    return;
}

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
