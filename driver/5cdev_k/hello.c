#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>

MODULE_LICENSE("GPL");

static int major = 200;

/*  inode 存放不变的信息； 
    file 存放可变的信息 
    每打开一次文件都会创建一个file
*/
static int hello_open(struct inode *inode, struct file *filep)
{
    printk("kk hello open\n");
    return 0;
}
static int hello_release (struct inode *inode, struct file *filep)
{
    printk("kk hello release\n");
    return 0;
}

static struct file_operations hello_ops={
    .open = hello_open,
    .release = hello_release,
};

static int hello_init(void)
{
    int res, error;
    printk("Hello_init \n");
    res = register_chrdev(major, "kk dev", &hello_ops);
    if(res < 0)
    {
        printk("register_chrdev fail, res = %d \n", res);
        return res;
    }
    
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit \n");
    unregister_chrdev(major, "kk dev");
    return ;
}

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");        // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");   // 别名

module_init(hello_init);
module_exit(hello_exit);

