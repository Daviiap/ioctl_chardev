# Mock of a sev-guest device

This is an implementation of a mock sev-guest device driver using the Linux kernel module interface.

## Building the sev-guest device module:

To build the kernel module, navigate to `src/` and run:

```bash
make
```

After running the command, many files will be generated inside the directory, including `chardev.ko`, which is the kernel module file. To install it, run:

```bash
sudo insmod chardev.ko
```

Now, run `ls /dev/sev-guest` to verify if the device file has been created. If it is created, you can use [sev-guest tool](https://github.com/AMDESE/sev-guest) to make ioctl calls to the device.

## Removing module:

Navigate to `src/` and run:

```bash
sudo rmmod chardev
make clean
```
