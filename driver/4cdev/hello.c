#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static int major = 200;
static int minor = 0;
/* 字符设备 */
static struct cdev cdev;
/* 设备号 */
static dev_t devno;

static int hello_open(struct inode *inode, struct file *filep)
{
    printk("kk hello open\n");
    return 0;
}

static struct file_operations hello_ops={
    .open = hello_open,
};

static int hello_init(void)
{
    int res, error;
    printk("Hello_init \n");
    devno = MKDEV(major, minor);
    res = register_chrdev_region(devno, 1, "test");
    if(res < 0)
    {
        printk("register_chrdev_region fail, res = %d \n", res);
        return res;
    }
    cdev_init(&cdev, &hello_ops);
    error = cdev_add(&cdev, devno, 1);
    if(error < 0){
        printk("cdev add fail %d\n", error);
        unregister_chrdev_region(devno, 1);
        return error;
    }

    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit \n");
    cdev_del(&cdev);
    unregister_chrdev_region(devno, 1);
    return ;
}

MODULE_AUTHOR("LUKEKE");        // 作者
MODULE_DESCRIPTION("dev test"); // 描述
MODULE_ALIAS("Driver Learn");   // 别名

module_init(hello_init);
module_exit(hello_exit);

