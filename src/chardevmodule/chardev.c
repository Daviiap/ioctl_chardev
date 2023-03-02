#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include "chardev.h"

#define DEVICE_NAME "chardev"
#define CLASS_NAME "char"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple character device driver");

static dev_t dev_num;
static struct cdev cdev;
static struct class *chardev_class = NULL;

static int chardev_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "chardev: device opened\n");
    return 0;
}

static int chardev_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "chardev: device closed\n");
    return 0;
}

static ssize_t chardev_read(struct file *file, char *buf, size_t count, loff_t *pos) {
    printk(KERN_INFO "chardev: device read\n");
    return 0;
}

static ssize_t chardev_write(struct file *file, const char *buf, size_t count, loff_t *pos) {
    printk(KERN_INFO "chardev: device write\n");
    return count;
}

int last_val = -9999;

static long device_ioctl(struct file* file, unsigned int cmd, unsigned long arg) {
    int val;
    switch (cmd) {
        case MY_IOCTL_CMD:
            if (copy_from_user(&val, (int*) arg, sizeof(val))) {
                return -EFAULT;
            }
            // Do something with val...
            last_val = val;
            printk(KERN_INFO "my_ioctl_device: MY_IOCTL_CMD received with arg %d\n", val);
            break;
        case MY_IOCTL_CMD_2:
            printk(KERN_INFO "my_ioctl_device: MY_IOCTL_CMD received last arg: %d\n", last_val);
            break;
        default:
            return -ENOTTY;
    }
    return 0;
}

static struct file_operations chardev_fops = {
    .owner = THIS_MODULE,
    .open = chardev_open,
    .release = chardev_release,
    .read = chardev_read,
    .write = chardev_write,
    .unlocked_ioctl = device_ioctl,
};

static int __init chardev_init(void) {
    int ret;

    // Allocate a device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "chardev: failed to allocate device number\n");
        return ret;
    }

    // Create a device class
    chardev_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(chardev_class)) {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: failed to create device class\n");
        return PTR_ERR(chardev_class);
    }

    // Create a device file
    if (device_create(chardev_class, NULL, dev_num, NULL, DEVICE_NAME) == NULL) {
        class_destroy(chardev_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: failed to create device file\n");
        return -EFAULT;
    }

    // Initialize the character device
    cdev_init(&cdev, &chardev_fops);
    cdev.owner = THIS_MODULE;

    // Add the character device to the system
    ret = cdev_add(&cdev, dev_num, 1);
    if (ret < 0) {
        device_destroy(chardev_class, dev_num);
        class_destroy(chardev_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "chardev: failed to add character device\n");
        return ret;
    }

    printk(KERN_INFO "chardev: module loaded\n");
    return 0;
}

static void __exit chardev_exit(void) {
    cdev_del(&cdev);
    device_destroy(chardev_class, dev_num);
    class_destroy(chardev_class);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "chardev: module unloaded\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
