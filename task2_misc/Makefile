obj-m += counter_driver.o

ccflags-y += -march=armv7-a

KDIR = /home/nguyen-tran/embedded_lab1/kernel/linux-5.15
CROSS = arm-linux-gnueabihf-

all:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KDIR) M=$(PWD) modules

clean:
	make ARCH=arm CROSS_COMPILE=$(CROSS) -C $(KDIR) M=$(PWD) clean
