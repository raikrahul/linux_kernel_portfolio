obj-m := mapping_hw.o
KDIR := /lib/modules/$(shell uname -r)/build

all: modules user

modules:
	make -C $(KDIR) M=$(shell pwd) modules

user:
	gcc -o mapping_user mapping_user.c

clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -f mapping_user axiom_check.ko axiom_check.mod* axiom_check.o
