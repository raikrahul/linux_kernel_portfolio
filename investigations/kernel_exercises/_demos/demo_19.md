---
title: "Demo 19: COW Demo"
driver: cow_driver.c
---

# Demo 19: Copy-on-Write Demo

Demonstrate COW mechanism.

## COW Lifecycle

1. Fork: Share pages, mark R/W=0
2. Write: Page fault → copy → update PTE
3. Both processes now have separate pages

## Build & Load

```bash
cd demo_19_cow_demo
make
sudo insmod cow_driver.ko
cat /proc/demo_cow
sudo rmmod cow_driver
```

## Source

[demo_19_cow_demo/](https://github.com/raikrahul/data_cpp/tree/master/demo_19_cow_demo)
