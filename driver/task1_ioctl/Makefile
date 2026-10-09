obj-m += asgn1_driver.o
ccflags-y := -march=armv7-a

KDIR ?= /home/nguyen-tran/embedded_lab1/kernel/linux-5.15
PWD := $(shell pwd)

CC := arm-linux-gnueabihf-gcc

all:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=$(PWD) modules
	$(CC) test_app.c -o test_app

clean:
	$(MAKE) ARCH=arm CROSS_COMPILE=arm-linux-gnueabihf- -C $(KDIR) M=$(PWD) clean
	rm -f test_app

