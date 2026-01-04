obj-m := vma_lookup_driver.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: modules user

modules:
	make -C $(KDIR) M=$(PWD) modules

user:
	gcc -o mmap_no_fault mmap_no_fault.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f mmap_no_fault
