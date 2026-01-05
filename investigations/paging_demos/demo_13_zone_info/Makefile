obj-m += zone_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

test:
	sudo insmod zone_driver.ko
	cat /proc/demo_zones
	sudo rmmod zone_driver
