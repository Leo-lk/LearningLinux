#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");

static int major = 200;
static int minor = 0;
static dev_t devno;
struct class * cls;
struct device * class_dev;
#define KMAXLEN 32
static char test_buf[KMAXLEN + 1] = "kernel test -----";


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
static ssize_t hello_read (struct file *filep, char __user *buf, size_t size, loff_t *pos)
{
    int error;

    if(size > strlen(test_buf))
        size = strlen(test_buf);
    if(copy_to_user(buf, test_buf, size))
    {
        error = -EFAULT;
        return error;
    }
    return size;
}
static ssize_t hello_write (struct file *filep, const char __user *buf, size_t size, loff_t *pos)
{
    int error;

    if(size > KMAXLEN)
        size = KMAXLEN;
    memset(test_buf, 0, sizeof(test_buf));
    if(copy_from_user(test_buf, buf, size))
    {
        error = -EFAULT;
        return error;
    }
    printk("hello_write kbuf: %s\n", test_buf);
    return size;
}




static struct file_operations hello_ops={
    .open = hello_open,
    .release = hello_release,
    .read = hello_read,
    .write = hello_write,
};

static int hello_init(void)
{
    int res;
    printk("Hello_init \n");
    res = register_chrdev(major, "kk dev", &hello_ops);
    if(res < 0){
        printk("register_chrdev fail, res = %d \n", res);
        return res;
    }
    
    cls = class_create(THIS_MODULE, "hellocls");
    if(IS_ERR (cls)){
        printk("class_create failed\n");
        res = PTR_ERR(cls);
        goto out_err_1;
    }
    devno = MKDEV(major, minor);
    /* 在/sys/class/hellocls创建hellodevice设备 */
    class_dev = device_create(cls, NULL, devno, NULL, "hellodevice");
    if(IS_ERR (class_dev)){
        printk("device_create failed\n");
        res = PTR_ERR(class_dev);
        goto out_err_2;
    }
    
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
    return ;
}

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");        // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");   // 别名

module_init(hello_init);
module_exit(hello_exit);

