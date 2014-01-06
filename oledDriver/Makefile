obj-m += oled.o

all:
	make -C ~/beagle/linux-dev_3.8/KERNEL ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- M=$(PWD) modules

clean:
	make -C ~/beagle/linux-dev_3.8/KERNEL ARCH=arm CROSS_COMPILE=arm-linux-gnueabi- M=$(PWD) clean
