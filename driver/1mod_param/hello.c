#include <linux/init.h>
#include <linux/module.h>
// #include <linux/kdev.h>
// #include <linux/cdev.h>
// #include <linux/fs.h>

/* 内核许可声明 */
MODULE_LICENSE("GPL");

static int num = 100;
static char *dbg = "st\n";
static int myarry[3] = {1, 2, 3};

static int hello_init(void)
{
    printk("Hello_init dbg = %s\r\n ", dbg);
    printk("Hello_init array = %d, %d, %d\r\n ", myarry[0],myarry[1],myarry[2]);
    return 0;
}

static void hello_exit(void)
{
    printk("Hello_exit num = %d\r\n", num);
    return ;
}

module_param(dbg, charp, S_IRUGO | S_IWUSR);
module_param_named(num_out, num, int, S_IRUGO | S_IWUSR);
module_param_array(myarry, int, NULL, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(dbg, "    this is a string print");
MODULE_PARM_DESC(num_out, "this is a int number");
MODULE_PARM_DESC(myarry, " this is a int array, 3 len");

module_init(hello_init);
module_exit(hello_exit);

//dmesg -c
//insmod hello.ko num_out=666 dbg="test" myarry=2,3,5
//rmmmod hello
//dmesg
