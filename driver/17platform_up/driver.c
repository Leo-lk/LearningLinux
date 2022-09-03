#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

/* 匹配成功后调用 */
int hello_probe(struct platform_device *pdev)
{
    printk("driver match ok \n");
    printk("mem = 0x%x\n", pdev->resource[0].start);
    printk("irq = %d\n", pdev->resource[1].start);
    /* 申请内存 */
    /* 注册中断 */
    return 0;
}
/* 驱动或设备一方卸载后调用 */
int hello_remove(struct platform_device *pdev)
{
    printk("driver hello remove \n");
    return 0;
}

struct platform_driver hello_driver = {
    .driver.name = "hello",
    .probe = hello_probe,
    .remove = hello_remove,
};

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

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("register_chrdev test"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
