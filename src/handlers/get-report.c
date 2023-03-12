#include <linux/uaccess.h>
#include "../snp/sev-guest.h"
#include "../snp/psp-sev.h"
#include "../snp/attestation.h"

int read_file_bytes(const char *filename, char *buffer, int length)
{
    struct file *fp;
    loff_t pos = 0;
    int bytes_read = 0;

    // open the file in read mode
    fp = filp_open(filename, O_RDONLY, 0);
    if (fp == NULL)
        return -ENOENT; // file not found error

    // read bytes from the file
    bytes_read = kernel_read(fp, buffer, length, &pos);

    // close the file
    filp_close(fp, NULL);

    return bytes_read;
}

static int handle_get_report(unsigned long guest_request_ptr)
{
    printk(KERN_INFO "sev-guest: received get attestation report");

    if (guest_request_ptr == 0)
    {
        printk(KERN_INFO "sev-guest: error: arg is NULL");
        return -EINVAL;
    }

    struct snp_guest_request_ioctl ioctl_request;
    if (copy_from_user(&ioctl_request, (int *)guest_request_ptr, sizeof(ioctl_request)))
    {
        printk(KERN_INFO "sev-guest: error copying request data from user.");
        return -EFAULT;
    }

    struct snp_report_resp resp;
    struct msg_report_resp *report_resp = (struct msg_report_resp *)&resp.data;
    if (copy_from_user(&resp, (int *)ioctl_request.resp_data, sizeof(resp)))
    {
        printk(KERN_INFO "sev-guest: error copying request data from user.");
        return -EFAULT;
    }

    report_resp->status = 0;
    report_resp->report_size = sizeof(report_resp->report);

    char buffer[1184];
    int bytes_read = read_file_bytes("/report.bin", buffer, 1184);
    if (bytes_read < 0)
    {
        printk(KERN_ERR "Error reading file\n");
        return bytes_read;
    }

    memcpy(&report_resp->report, &buffer, sizeof(buffer));

    if (copy_to_user((int *)guest_request_ptr, &ioctl_request, sizeof(ioctl_request)))
    {
        printk(KERN_INFO "sev-guest: error");
        return -EFAULT;
    }

    if (copy_to_user((int *)ioctl_request.resp_data, &resp, sizeof(resp)))
    {
        printk(KERN_INFO "sev-guest: error");
        return -EFAULT;
    }

    return 0;
}
