#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

struct foo_obj {
	struct kobject kobj;
	int foo;
	int baz;
	int bar;
};
/* 从类型为kobj的结构体成员的指针x，获取该foo_obj类型结构体的指针 */
#define to_foo_obj(x) container_of(x, struct foo_obj, kobj)

/* 自定义属性，继承自attribute */
struct foo_attribute {
	struct attribute attr; /* 包含name 和 mode 成员变量*/
	ssize_t (*show)(struct foo_obj *foo, struct foo_attribute *attr, char *buf);
	ssize_t (*store)(struct foo_obj *foo, struct foo_attribute *attr, const char *buf, size_t count);
};
#define to_foo_attr(x) container_of(x, struct foo_attribute, attr)

/*
 * 每当与已注册kobject关联的sysfs文件上的show函数被用户调用时，这个函数就会被sysfs调用。
 * 需要把传入的kobj转置为自己的kobj子类，然后调用这个特定对象的show函数 */
static ssize_t foo_attr_show(struct kobject *kobj,
			     struct attribute *attr,
			     char *buf)
{
	struct foo_attribute *attribute;
	struct foo_obj *foo;
	/* 获取attr的子类，以调用子类中的操作函数 */
	attribute = to_foo_attr(attr);
	foo = to_foo_obj(kobj);

	if (!attribute->show)
		return -EIO;
	
	return attribute->show(foo, attribute, buf);
}

/* 通过sysfs写入属性文件时被调用 */
static ssize_t foo_attr_store(struct kobject *kobj,
			      struct attribute *attr,
			      const char *buf, size_t len)
{
	struct foo_attribute *attribute;
	struct foo_obj *foo;

	attribute = to_foo_attr(attr);
	foo = to_foo_obj(kobj);

	if (!attribute->store)
		return -EIO;

	return attribute->store(foo, attribute, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static const struct sysfs_ops foo_sysfs_ops = {
	.show = foo_attr_show,
	.store = foo_attr_store,
};

/* 属性的读取函数，每个属性可以有不同的读取 */
/* (自定义)绑定在在attribute的子类中，foo_attr_show时
 * 通过传入的attribute获取这个子类后调用var_show */
static ssize_t var_show(struct foo_obj *foo_obj, struct foo_attribute *attr,
		      char *buf)
{
	int var;
	if (strcmp(attr->attr.name, "len_attr") == 0)
		var = foo_obj->foo;
	else if (strcmp(attr->attr.name, "derict_attr") == 0)
		var = foo_obj->baz;
	else
		var = foo_obj->bar;
	
	return sprintf(buf, "%d\n", var);
}
/* 属性的写入函数，每个属性可以有不同的写入 */
static ssize_t var_store(struct foo_obj *foo_obj, struct foo_attribute *attr,
		       const char *buf, size_t count)
{
	int var, ret;

	ret = kstrtoint(buf, 10, &var);
	if (ret < 0)
		return ret;

	if (strcmp(attr->attr.name, "len_attr") == 0)
		foo_obj->foo = var;
	else if (strcmp(attr->attr.name, "derict_attr") == 0)
		foo_obj->baz = var;
	else
		foo_obj->bar = var;
	
	return count;
}
/* Sysfs attributes cannot be world-writable.   
 * __ATTR(name, mode, show, store) : 生成一个包含属性的结构体，以及相关的访问函数。
 * 这样，可以将属性与对应的读写函数关联起来，并在 sysfs 中创建相应的属性文件，用于读取或写入属性值。*/
static struct foo_attribute foo_attribute =
	__ATTR(len_attr, 0664, var_show, var_store);
static struct foo_attribute baz_attribute =
	__ATTR(derict_attr, 0664, var_show, var_store);
static struct foo_attribute bar_attribute =
	__ATTR(depth_attr, 0664, var_show, var_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *foo_default_attrs[] = {
	&foo_attribute.attr,
	&baz_attribute.attr,
	&bar_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static void foo_release(struct kobject *kobj)
{
	struct foo_obj *foo;

	foo = to_foo_obj(kobj);
	kfree(foo);
}

/*
 * 可以定义所属kset的kobject的一些行为到default_attrs和sysfs_ops属性中，*/
static struct kobj_type foo_ktype = {
    .sysfs_ops = &foo_sysfs_ops,
    .release = foo_release,
    .default_attrs = foo_default_attrs,
};

static struct kset *example_kset;
static struct foo_obj *foo_obj;
static struct foo_obj *bar_obj;
static struct foo_obj *baz_obj;

static struct foo_obj *create_foo_obj(const char *name)
{
	struct foo_obj *foo;
	int retval;

	/* allocate the memory for the whole object */
	foo = kzalloc(sizeof(*foo), GFP_KERNEL);
	if (!foo)
		return NULL;
	
	/* 初始化kobject之前先确定所属kset */
	foo->kobj.kset = example_kset;

	/* 初始化kobject添加到kernel中，并关联ktype，会在sysfs中创建名为name的kobject文件夹
	 * 第三个参数是父kobj，由于已确定kset，写为NULL */
	retval = kobject_init_and_add(&foo->kobj, &foo_ktype, NULL, "%s", name);
	if (retval) {
		kobject_put(&foo->kobj);
		return NULL;
	}
	/* 通知用户空间有一个新的内核对象（kobject）已经被添加到 sysfs 中。
	 * 这对于用户空间的监控和管理工具来说是很有用的 */
	kobject_uevent(&foo->kobj, KOBJ_ADD);

	return foo;
}

static void destroy_foo_obj(struct foo_obj *foo)
{
	kobject_put(&foo->kobj);
}

static int __init example_init(void)
{
    /* 创建一个名为 "kset_example" 的kset, 路径在/sys/kernel/ */
    example_kset = kset_create_and_add("kset_example", NULL, kernel_kobj);
    if (!example_kset)
    	return -ENOMEM;
    /* 在已定义的kset下新增kobject */
    foo_obj = create_foo_obj("foo");
    if (!foo_obj)
    	goto foo_error;
    
    bar_obj = create_foo_obj("bar");
    if (!bar_obj)
    	goto bar_error;
    
    baz_obj = create_foo_obj("baz");
    if (!baz_obj)
    	goto baz_error;
    
    return 0;

baz_error:
	destroy_foo_obj(bar_obj);
bar_error:
	destroy_foo_obj(foo_obj);
foo_error:
	kset_unregister(example_kset);
	return -EINVAL;
}

static void __exit example_exit(void)
{
	destroy_foo_obj(baz_obj);
	destroy_foo_obj(bar_obj);
	destroy_foo_obj(foo_obj);
	kset_unregister(example_kset);
}

MODULE_AUTHOR("LUKEKE");        // 作者
MODULE_DESCRIPTION("kset test"); // 描述
MODULE_ALIAS("kset Learn");   // 别名

module_init(example_init);
module_exit(example_exit);

