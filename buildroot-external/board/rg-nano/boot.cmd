setenv bootargs "console=ttyS0,115200 console=tty0 root=/dev/mmcblk0p2 rootwait rw rootfstype=ext4 loglevel=7"
load mmc 0:1 0x41000000 zImage
load mmc 0:1 0x41800000 sun8i-v3s-anbernic-rg-nano.dtb
fdt addr 0x41800000
fdt set /soc/usb@1c19000 dr_mode "peripheral"
bootz 0x41000000 - 0x41800000
