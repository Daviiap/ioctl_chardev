#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/sev-guest.h>

int main()
{
    int fd;
    struct snp_guest_request_ioctl arg; // Example argument for SNP_GET_REPORT
    memset(&arg, 0, sizeof(arg));

    fd = open("/dev/sev-guest", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        exit(1);
    }

    printf("%d\n", arg.msg_version);

    if (ioctl(fd, SNP_GET_REPORT, &arg) < 0)
    {
        perror("ioctl");
        exit(1);
    }

    printf("%d\n", arg.msg_version);
    printf("ioctl command sent\n");

    close(fd);
    return 0;
}
