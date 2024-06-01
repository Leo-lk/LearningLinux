#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>


MODULE_LICENSE("GPL");

struct temp_obj {
	struct kobject kobj;
	int len;
	int dep;
	int enb;
};
/* 从类型为kobj的结构体成员的指针x，获取该temp_obj类型结构体的指针 */
#define to_temp_obj(x) container_of(x, struct temp_obj, kobj)

static void temp_release(struct kobject *kobj)
{
	struct temp_obj *foo;

	foo = to_temp_obj(kobj);
	kfree(foo);
}

/*
可以定义所属kset的kobject的一些行为到default_attrs和sysfs_ops属性中，现在仅
使用release
 */
static struct kobj_type foo_ktype = {
    // .sysfs_ops = &foo_sysfs_ops,
    .release = temp_release,
    // .default_attrs = foo_default_attrs,
};

static struct kset *example_kset;
static struct temp_obj *foo_obj;
static struct temp_obj *bar_obj;
static struct temp_obj *baz_obj;

static struct temp_obj *create_temp_obj(const char *name)
{
	struct temp_obj *foo;
	int retval;

	/* allocate the memory for the whole object */
	foo = kzalloc(sizeof(*foo), GFP_KERNEL);
	if (!foo)
		return NULL;

	/* 初始化kobject之前先确定所属kset */
	foo->kobj.kset = example_kset;

	/* 初始化kobject并添加到kernel中，第三个参数是父kobj，由于已确定kset，写为NULL */
	retval = kobject_init_and_add(&foo->kobj, &foo_ktype, NULL, "%s", name);
	if (retval) {
		kobject_put(&foo->kobj);
		return NULL;
	}
	/*
	 * We are always responsible for sending the uevent that the kobject
	 * was added to the system.
	 */
	kobject_uevent(&foo->kobj, KOBJ_ADD);

	return foo;
}

static void destroy_temp_obj(struct temp_obj *foo)
{
	kobject_put(&foo->kobj);
	kfree(foo);
}

static int __init example_init(void)
{
    /* 创建一个名为 "kset_example" 的kset, 路径在/sys/kernel/ */
    example_kset = kset_create_and_add("kset_example", NULL, kernel_kobj);
    if (!example_kset)
    	return -ENOMEM;
    /* 在已定义的kset下新增kobject */
    foo_obj = create_temp_obj("foo");
    if (!foo_obj)
    	goto foo_error;
    
    bar_obj = create_temp_obj("bar");
    if (!bar_obj)
    	goto bar_error;
    
    baz_obj = create_temp_obj("baz");
    if (!baz_obj)
    	goto baz_error;
    
    return 0;

baz_error:
	destroy_temp_obj(bar_obj);
bar_error:
	destroy_temp_obj(foo_obj);
foo_error:
	kset_unregister(example_kset);
	return -EINVAL;
}

static void __exit example_exit(void)
{
	destroy_temp_obj(baz_obj);
	destroy_temp_obj(bar_obj);
	destroy_temp_obj(foo_obj);
	kset_unregister(example_kset);
}

MODULE_AUTHOR("LUKEKE");        // 作者
MODULE_DESCRIPTION("kset test"); // 描述
MODULE_ALIAS("kset Learn");   // 别名

module_init(example_init);
module_exit(example_exit);

