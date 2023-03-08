#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/sev-guest.h>

#define DEVICE_NAME "sev-guest"
#define CLASS_NAME "char"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Davi Andrade Pontes");
MODULE_DESCRIPTION("Mock of a sev-snp guest device.");

static dev_t dev_num;
static struct cdev cdev;
static struct class *chardev_class = NULL;

static int handle_get_report(unsigned long arg)
{
    printk(KERN_INFO "sev-guest: receiver get attestation report");

    if (arg == 0)
    {
        printk(KERN_INFO "sev-guest: error: arg is NULL");
        return -EINVAL;
    }

    struct snp_guest_request_ioctl request;
    if (copy_from_user(&request, (int *)arg, sizeof(request)))
    {
        printk(KERN_INFO "sev-guest: error copying request data from user.");
        return -EFAULT;
    }

    request.msg_version = 2;

    if (copy_to_user((int *)arg, &request, sizeof(request)))
    {
        printk(KERN_INFO "sev-guest: error");
        return -EFAULT;
    }

    return 0;
}

static long device_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int ret = 0;

    switch (cmd)
    {
    case SNP_GET_REPORT:
        ret = handle_get_report(arg);
        break;
    default:
        ret = -ENOTTY;
    }

    return ret;
}

static struct file_operations chardev_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = device_ioctl,
};

static int __init chardev_init(void)
{
    int ret;

    // Allocate a device number
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
    {
        printk(KERN_ERR "sev-guest: failed to allocate device number\n");
        return ret;
    }

    // Create a device class
    chardev_class = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(chardev_class))
    {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "sev-guest: failed to create device class\n");
        return PTR_ERR(chardev_class);
    }

    // Create a device file
    if (device_create(chardev_class, NULL, dev_num, NULL, DEVICE_NAME) == NULL)
    {
        class_destroy(chardev_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "sev-guest: failed to create device file\n");
        return -EFAULT;
    }

    // Initialize the character device
    cdev_init(&cdev, &chardev_fops);
    cdev.owner = THIS_MODULE;

    // Add the character device to the system
    ret = cdev_add(&cdev, dev_num, 1);
    if (ret < 0)
    {
        device_destroy(chardev_class, dev_num);
        class_destroy(chardev_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ERR "sev-guest: failed to add character device\n");
        return ret;
    }

    printk(KERN_INFO "sev-guest: module loaded\n");
    return 0;
}

static void __exit chardev_exit(void)
{
    cdev_del(&cdev);
    device_destroy(chardev_class, dev_num);
    class_destroy(chardev_class);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "sev-guest: module unloaded\n");
}

module_init(chardev_init);
module_exit(chardev_exit);
