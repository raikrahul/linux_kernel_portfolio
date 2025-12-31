obj-m := anon_file_hw.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(shell pwd) modules
	gcc -o anon_user anon_user.c

clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -f anon_user test_mapped_file.txt
