#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "chardev.h"

int main()
{
    int fd;
    int arg = 42; // Example argument for MY_IOCTL_CMD

    fd = open("/dev/chardev", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    if (ioctl(fd, MY_IOCTL_CMD, &arg) < 0)
    {
        perror("ioctl");
        exit(1);
    }

    printf("ioctl command sent with arg %d\n", arg);

    if (ioctl(fd, MY_IOCTL_CMD_2, &arg) < 0)
    {
        perror("ioctl");
        exit(1);
    }

    close(fd);
    return 0;
}
