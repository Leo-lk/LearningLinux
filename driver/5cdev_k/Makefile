ifneq ($(KERNELRELEASE),)
obj-m:=hello.o 
else
KDIR ?=/home/eden/tools/100ask_imx6ull-qemu/linux-4.9.88
#KDIR ?=/lib/modules/$(shell uname -r)/build
PWD  :=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -f *.ko *.o *.mod.o *.symvers *.cmd .*.cmd  *.mod.c *.order
endif
