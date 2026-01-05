obj-m += pml4_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

test:
	sudo insmod pml4_driver.ko
	cat /proc/demo_pml4
	sudo rmmod pml4_driver
