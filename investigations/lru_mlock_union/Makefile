obj-m := lru_mlock_hw.o
KDIR := /lib/modules/$(shell uname -r)/build

all: modules user

modules:
	make -C $(KDIR) M=$(shell pwd) modules

user:
	gcc -o mlock_user mlock_user.c

clean:
	make -C $(KDIR) M=$(shell pwd) clean
	rm -f mlock_user
