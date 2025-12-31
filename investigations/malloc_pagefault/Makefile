obj-m := pagefault_trace.o

KDIR := /lib/modules/$(shell uname -r)/build

all: modules userspace

modules:
	make -C $(KDIR) M=$(shell pwd) modules

userspace:
	gcc -o malloc_user malloc_user.c

clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -f malloc_user
