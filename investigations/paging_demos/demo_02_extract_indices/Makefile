obj-m += indices_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

test:
	sudo insmod indices_driver.ko
	cat /proc/demo_indices
	sudo rmmod indices_driver
