ifneq ($(KERNELRELEASE),)
obj-m:=hello.o
else
KDIR :=/home/eden/tools/100ask_imx6ull-qemu/linux-4.9.88
PWD  :=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
endif
