#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

extern int num ;
extern void show(void);

static int hello_init(void)
{
    printk("Hello_init b %d\n", num);
    show();
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit b %d\n", num);
    return ;
}
module_init(hello_init);
module_exit(hello_exit);

