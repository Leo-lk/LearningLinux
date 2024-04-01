ifneq ($(KERNELRELEASE),)
obj-m:=device.o driver.o 
else
KDIR ?=/lib/modules/$(shell uname -r)/build
PWD  :=$(shell pwd)
all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm -rf *.ko *.o *.mod.o *.symvers *.cmd .*.cmd  .tmp*  *.mod.c *.order
endif
