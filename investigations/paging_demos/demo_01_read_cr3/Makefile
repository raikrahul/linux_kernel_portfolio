obj-m += cr3_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

test:
	sudo insmod cr3_driver.ko
	cat /proc/demo_cr3
	sudo rmmod cr3_driver
