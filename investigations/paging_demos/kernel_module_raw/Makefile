obj-m += raw_pagewalk.o
obj-m += prove_direct_map.o

KDIR := /lib/modules/$(shell uname -r)/build

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

# USAGE:
# 1. Fill in TODOs in raw_pagewalk.c
# 2. make
# 3. sudo insmod raw_pagewalk.ko
# 4. dmesg | tail -20
# 5. sudo rmmod raw_pagewalk
