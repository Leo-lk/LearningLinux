#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

static int major = 200;
static int minor = 0;

static dev_t devno;

static int hello_init(void)
{
    int res;
    printk("Hello_init \n");
    devno = MKDEV(major, minor);
    res = register_chrdev_region(devno, 1, "test");
    if(res < 0)
    {
        printk("register_chrdev_region fail, res = %d \n", res);
        return res;
    }
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit \n");
    unregister_chrdev_region(devno, 1);
    return ;
}

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");        // 作者
MODULE_DESCRIPTION("dev test"); // 描述
MODULE_ALIAS("Driver Learn");   // 别名

module_init(hello_init);
module_exit(hello_exit);

