obj-m := page_refcount_bug.o buddy_fragment.o gfp_context_bug.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

# Individual targets
page_refcount_bug:
	make -C $(KDIR) M=$(PWD) page_refcount_bug.ko

buddy_fragment:
	make -C $(KDIR) M=$(PWD) buddy_fragment.ko

gfp_context_bug:
	make -C $(KDIR) M=$(PWD) gfp_context_bug.ko
