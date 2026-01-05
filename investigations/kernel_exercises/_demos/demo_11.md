---
title: "Demo 11: Dump PML4"
driver: pml4_driver.c
---

# Demo 11: Dump PML4 Table

Dump all 512 PML4 entries showing present/not present.

## PML4 Layout

- Entries 0-255: User space
- Entries 256-511: Kernel space

## Build & Load

```bash
cd demo_11_dump_pml4
make
sudo insmod pml4_driver.ko
cat /proc/demo_pml4
sudo rmmod pml4_driver
```

## Source

[demo_11_dump_pml4/](https://github.com/raikrahul/data_cpp/tree/master/demo_11_dump_pml4)
