#include <linux/uaccess.h>

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
