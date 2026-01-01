# Makefile for MM Exercise and Page Fault Observer

obj-m += mm_exercise_hw.o
obj-m += pagefault_observer.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all: modules user

modules:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user:
	gcc -o mm_exercise_user mm_exercise_user.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f mm_exercise_user
