#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");

/*
在/sys/kernel/目录下创建名为kset_example的kset
kset中创建三个obj，读写它们的三个属性：

[root@ATK-DLRV1126:/sys/kernel/kset_example]# ls
bar-kojb  baz-kojb  foo-kojb
[root@ATK-DLRV1126:/sys/kernel/kset_example]# echo 1 > foo-kojb/len-attr
[root@ATK-DLRV1126:/sys/kernel/kset_example]# echo 3 > foo-kojb/dep-attr 
[root@ATK-DLRV1126:/sys/kernel/kset_example]# echo 9 > baz-kojb/dep-attr 
[root@ATK-DLRV1126:/sys/kernel/kset_example]# cat foo-kojb/len-attr 
foo-kojb, len-attr=1
[root@ATK-DLRV1126:/sys/kernel/kset_example]# cat foo-kojb/dep-attr 
foo-kojb, dep-attr=3
[root@ATK-DLRV1126:/sys/kernel/kset_example]# cat baz-kojb/dep-attr 
baz-kojb, dep-attr=9
*/

struct temp_obj {
	struct kobject kobj;
	int len;
	int dep;
	int enb;
};
/* 从名为kobj的结构体成员的指针x，获取该temp_obj类型结构体的指针 */
#define to_temp_obj(x) container_of(x, struct temp_obj, kobj)

struct temp_attr {
	struct attribute attr;
	ssize_t (*show)(struct temp_obj *temp, struct temp_attr *attr, char *buf);
	ssize_t (*store)(struct temp_obj *temp, struct temp_attr *attr, const char *buf, size_t count);
};
/* 从名为kobj的结构体成员的指针x，获取该temp_obj类型结构体的指针 */
#define to_temp_attr(x) container_of(x, struct temp_attr, attr)

/*
 * 通过sysfs读取属性文件时被sysfs调用。
 * 需要把传入的kobj转置为自己的kobj子类，然后调用这个特定对象的show函数 */
static ssize_t temp_attr_show(struct kobject *kobj,
			     struct attribute *attr,
			     char *buf)
{
	struct temp_obj *tempobj;
	struct temp_attr *tempattr;
	/* 获取kobj的子类实例 */
	tempobj = to_temp_obj(kobj);
	tempattr = to_temp_attr(attr);
	/* 打印出想要显示的 */
	/* 打印行为也可以是kobj子类或attr子类中定义的ops */
	return tempattr->show(tempobj, tempattr, buf);
}

/* 通过sysfs写入属性文件时被调用 */
/* 可以根据传入的attr来区分不同的属性 */
/* kobj是这个属性的拥有者 */
static ssize_t temp_attr_store(struct kobject *kobj,
			      struct attribute *attr,
			      const char *buf, size_t len)
{
	struct temp_obj *tempobj;
	struct temp_attr *tempattr;
	/* 获取kobj的子类实例 */
	tempobj = to_temp_obj(kobj);
	tempattr = to_temp_attr(attr);
	/* 把传入的数写入想要修改的变量中 */
	/* 打印行为也可以是kobj子类或attr子类中定义的ops */
	return tempattr->store(tempobj, tempattr, buf, len);
}

/* Our custom sysfs_ops that we will associate with our ktype later on */
static const struct sysfs_ops temp_sysfs_ops = {
	.show = temp_attr_show,
	.store = temp_attr_store,
};

/* 为每一个属性文件都定义一个读写函数 */
#define DEFINE_ATTR(arg) \
ssize_t show_##arg(struct temp_obj *temp, struct temp_attr *attr, char *buf)\
{\
	return sprintf(buf, "%s, %s=%d\n", temp->kobj.name, attr->attr.name, temp->arg);\
}\
ssize_t store_##arg(struct temp_obj *temp, struct temp_attr *attr, const char *buf, size_t count)\
{\
	int ret, var;\
	/* 把传入buf中的字符串转换为整型 */\
	ret = kstrtoint(buf, 10, &var);\
	if (ret < 0)\
		return ret;\
	temp->arg = var;\
	return count;\
}\
struct temp_attr attr_##arg = {\
	.attr ={\
		.name = #arg"-attr",\
		.mode = 0644,\
	},\
	.show = show_##arg,\
	.store = store_##arg,\
};

DEFINE_ATTR(len)
DEFINE_ATTR(dep)
DEFINE_ATTR(enb)
#undef DEFINE_ATTR

/*
 * 属性文件数组，保证这些属性可以一同创建和销毁
 */
static struct attribute *temp_default_attrs[] = {
	&attr_len.attr,
	&attr_dep.attr,
	&attr_enb.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

static void temp_release(struct kobject *kobj)
{
	struct temp_obj *temp;

	temp = to_temp_obj(kobj);
	kfree(temp);
}

/*
 * 可以定义所属kset的kobject的一些属性和行为到default_attrs和sysfs_ops属性中，*/
static struct kobj_type temp_ktype = {
    .sysfs_ops = &temp_sysfs_ops,
    .release = temp_release,
    .default_attrs = temp_default_attrs,
};

static struct kset *example_kset;
static struct temp_obj *foo_obj;
static struct temp_obj *bar_obj;
static struct temp_obj *baz_obj;

static struct temp_obj *create_kobj(struct kset * kset, const char *name)
{
	struct temp_obj *temp;
	int retval;

	/* allocate the memory for the whole object */
	temp = kzalloc(sizeof(*temp), GFP_KERNEL);
	if (!temp)
		return NULL;
	
	/* 初始化kobject之前先确定所属kset */
	temp->kobj.kset = kset;

	/* 初始化kobject添加到kernel中，并关联ktype，会在sysfs中创建名为name的kobject文件夹
	 * 第三个参数是父kobj，由于已确定kset，写为NULL */
	retval = kobject_init_and_add(&temp->kobj, &temp_ktype, NULL, "%s", name);
	if (retval) {
		kobject_put(&temp->kobj);
		return NULL;
	}
	/* 通知用户空间有一个新的内核对象（kobject）已经被添加到 sysfs 中。
	 * 这对于用户空间的监控和管理工具来说是很有用的 */
	kobject_uevent(&temp->kobj, KOBJ_ADD);

	return temp;
}

static void destroy_temp_obj(struct temp_obj *temp)
{
	kobject_put(&temp->kobj);
	kfree(temp);
}

static int __init example_init(void)
{
    /* 创建一个名为 "kset_example" 的kset, 路径在/sys/kernel/ */
    example_kset = kset_create_and_add("kset_example", NULL, kernel_kobj);
    if (!example_kset)
    	return -ENOMEM;
    /* 在已定义的kset下新增kobject */
    foo_obj = create_kobj(example_kset, "foo-kojb");
    if (!foo_obj)
    	goto foo_error;
    
    bar_obj = create_kobj(example_kset, "bar-kojb");
    if (!bar_obj)
    	goto bar_error;
    
    baz_obj = create_kobj(example_kset, "baz-kojb");
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

