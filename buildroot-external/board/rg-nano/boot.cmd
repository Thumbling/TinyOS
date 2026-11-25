setenv bootargs "console=ttyS0,115200 root=/dev/mmcblk0p2 rootwait rw rootfstype=ext4 quiet loglevel=3"
load mmc 0:1 0x42000000 zImage
load mmc 0:1 0x43000000 sun8i-v3s-anbernic-rg-nano.dtb
bootz 0x42000000 - 0x43000000
