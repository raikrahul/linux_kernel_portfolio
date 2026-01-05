obj-m := lru_mlock_hw.o
KDIR := /lib/modules/$(shell uname -r)/build

all: modules user

modules:
	make -C $(KDIR) M=$(PWD) modules

user:
	gcc -o mlock_user mlock_user.c

clean:
	make -C $(KDIR) M=$(PWD) clean
	rm -f mlock_user
