#include <linux/init.h>
#include <linux/module.h>
// #include <linux/kdev.h>
// #include <linux/cdev.h>
// #include <linux/fs.h>

/* 内核许可声明 */
MODULE_LICENSE("GPL");

static int num = 100;
static char *dbg = "st\n";

static int hello_init(void)
{
    printk("Hello_init dbg = %s\n ", dbg);
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit num = %d\n", num);
    return ;
}

module_param(dbg, charp, 0644);
module_param_named(num_out, num, int, 0644);

module_init(hello_init);
module_exit(hello_exit);

//dmesg -c
//insmod hello num_out=666 dbg="debug test"
//rmmmod hello
//dmesg
