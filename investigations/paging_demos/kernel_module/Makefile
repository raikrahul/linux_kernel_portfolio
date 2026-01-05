# Makefile for pagewalk kernel module
# 
# Build:   make
# Install: sudo insmod pagewalk_driver.ko
# Use:     echo "0x7f0000000000" > /proc/pagewalk && catn  /proc/pagewalk
# Remove:  sudo rmmod pagewalk_driver
# Clean:   make clean

obj-m := pagewalk_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install: all
	sudo insmod pagewalk_driver.ko

uninstall:
	sudo rmmod pagewalk_driver

test: install
	@echo "Testing pagewalk driver..."
	@echo "stack" > /proc/pagewalk
	@cat /proc/pagewalk

.PHONY: all clean install uninstall test
