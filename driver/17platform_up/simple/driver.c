#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

MODULE_LICENSE("GPL");

struct local_dev_t{
    struct platform_device *pdev;
};

/* 匹配成功后调用 */
int hello_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct local_dev_t *local_dev;
    
    printk("driver match ok \n");
    printk("mem = 0x%x\n", pdev->resource[0].start);
    printk("irq = %d\n", pdev->resource[1].start);
    /* 申请内存 */
    /* 注册中断 */

    local_dev = devm_kzalloc(&pdev->dev, sizeof(*local_dev), GFP_KERNEL);
    if (!local_dev) {
        ret = -ENOMEM;
        return ret;
    }


    /* 把local_dev放到pdev->dev.drvdata中 */
    platform_set_drvdata(pdev, &local_dev);


    return 0;
}
/* 驱动或设备一方卸载后调用 */
int hello_remove(struct platform_device *pdev)
{
    struct local_dev_t *local_dev = pdev->dev.driver_data;
    printk("driver hello remove \n");
    devm_kfree(&pdev->dev, local_dev);
    return 0;
}

struct platform_driver hello_driver = {
    .driver.name = "hello",
    .probe = hello_probe,
    .remove = hello_remove,
};
#if 0
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
module_init(hello_init);
module_exit(hello_exit);
#else
/* 等同于在init和exit中仅仅注册注销驱动 */
module_platform_driver(hello_driver);
#endif

MODULE_AUTHOR("LUKEKE <eden.lukeke@qq.com>");   // 作者
MODULE_DESCRIPTION("register_chrdev test");     // 描述
MODULE_ALIAS("Driver Learn");                   // 别名


