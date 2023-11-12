#include "rickroll.h"
#include "music.h"

#define DEVICE_NAME "rickroll"

static int major;
static atomic_t already_open = ATOMIC_INIT(0);
static struct class *cls;

static struct file_operations dev_fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
};

static struct miscdevice rickroll_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = DEVICE_NAME,
    .fops = &dev_fops,
    .mode = 0444,
};

static int __init rickroll_init(void) {
    major = misc_register(&rickroll_device);
    if(major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }

    cls = class_create(DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);
    pr_info("Device created on /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit rickroll_exit(void) {
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    misc_deregister(&rickroll_device);
}

static int device_open(struct inode *inode, struct file *file) {
    if(atomic_cmpxchg(&already_open, 0, 1))
        return -EBUSY;
    try_module_get(THIS_MODULE);
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    atomic_set(&already_open, 0);
    module_put(THIS_MODULE);
    return 0;
}

static ssize_t device_read(struct file *filp,
        char __user *buffer,
        size_t length,
        loff_t *offset) {
    ssize_t len = min(sizeof(rickroll)-*offset, length);
    if(copy_to_user(buffer, rickroll + *offset, len)) {
        return -EFAULT;
    }
    *offset += len;
    *offset %= sizeof(rickroll);
    return len;
}

static ssize_t device_write(struct file *filp,
        const char __user *buff,
        size_t len, loff_t *off) {
    pr_alert("This operation is not supported.\n");
    return -EINVAL;
}

module_init(rickroll_init);
module_exit(rickroll_exit);

MODULE_LICENSE("GPL");
