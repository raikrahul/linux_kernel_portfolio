obj-m += asid_driver.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

# USAGE:
# make
# sudo insmod asid_driver.ko
# cat /proc/asid_info
# sudo rmmod asid_driver
