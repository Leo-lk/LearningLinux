ifneq ($(KERNELRELEASE),)
obj-m:=device.o driver.o 
else
KDIR ?=/lib/modules/$(shell uname -r)/build
PWD  :=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	make -C $(KDIR) M=$(PWD) clean
endif
