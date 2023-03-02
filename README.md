# Dummy ioctl interface

This is an implementation of a dummy ioctl device driver using the Linux kernel module interface.

## Building the device module:

To build the kernel module, navigate to `src/chardevmodule` and run:

```bash
make
```

After running the command, many files will be generated inside the directory, including `chardev.ko`, which is the kernel module file. To install it, run:

```bash
sudo insmod chardev.ko
```

Now, run `ls /dev/chardev` to verify if the device file has been created.

## Making ioctl calls:

To test the device module, run the following command in a terminal:

```bash
dmesg -w
# ...
# [11429.799463] chardev: module loaded
```

This will display kernel logs.

The module accepts the ioctl commands described in `src/chardevmodule/chardev.h`. To test it, build `src/main.c`. From inside the `src` directory, run:

```bash
gcc -o main main.c && sudo ./main
```

You should see `ioctl command sent with arg 42` displayed in the terminal.

In the terminal where `dmesg -w` is running, you should see the following output:

```log
[11851.371355] chardev: device opened
[11851.371359] my_ioctl_device: MY_IOCTL_CMD received with arg 42
[11851.371404] my_ioctl_device: MY_IOCTL_CMD received last arg: 42
[11851.371406] chardev: device closed
```
