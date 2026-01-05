obj-m += flags_lab.o
obj-m += refcount_lab.o
obj-m += compound_lab.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
