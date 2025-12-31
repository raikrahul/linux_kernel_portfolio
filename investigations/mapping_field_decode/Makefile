obj-m := mapping_hw.o
KDIR := /lib/modules/$(shell uname -r)/build

all: modules user

modules:
	make -C $(KDIR) M=$(PWD) modules

user:
	gcc -o mapping_user mapping_user.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f mapping_user axiom_check.ko axiom_check.mod* axiom_check.o
