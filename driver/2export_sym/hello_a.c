#include <linux/init.h>
#include <linux/module.h>

MODULE_LICENSE("GPL");

static int num = 100;

static int hello_init(void)
{
    printk("Hello_init a \n");
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit a \n");
    return ;
}


void show(void) {
    printk("a show %d\n", num);
    return;
}

EXPORT_SYMBOL(show);
EXPORT_SYMBOL(num);

module_init(hello_init);
module_exit(hello_exit);

