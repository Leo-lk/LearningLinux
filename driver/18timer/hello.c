#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

struct timer_list my_timer;


void callback(struct timer_list * tm)
{
    printk("call back\n");
    tm->expires = jiffies + 2*HZ;
    add_timer(tm);
}


static int hello_init(void)
{
    timer_setup(&my_timer, callback, 0);
    my_timer.expires = jiffies + 2*HZ;
    add_timer(&my_timer);
    return 0;
}

static void hello_exit(void)
{
    del_timer_sync(&my_timer);
    return;
}

MODULE_AUTHOR("LUKEKE");                    // 作者
MODULE_DESCRIPTION("Timer"); // 描述
MODULE_ALIAS("Driver Learn");               // 别名

module_init(hello_init);
module_exit(hello_exit);
