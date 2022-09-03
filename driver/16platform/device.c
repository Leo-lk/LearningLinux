#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>


MODULE_LICENSE("GPL");

static void hello_release(struct device *dev)
{
    printk("device kk hello release\n");
    
    return ;
}

static struct platform_device hello_device = {
    .name = "hello",
    .id = -1,
    .dev.release = hello_release,/* 必须存在 */
};

static int hello_init(void)
{
    printk("device Hello_init \n");
    /* 注册 */
    return platform_device_register(&hello_device);
}

static void hello_exit(void)
{
    printk("device Hello_exit \n");
    /* 卸载 */
    platform_device_unregister(&hello_device);
    return;
}

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
