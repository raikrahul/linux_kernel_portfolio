---
title: "Demo 06: Extract Address"
driver: addr_driver.c
---

# Demo 06: Extract Physical Address

Extract physical address using different masks for page sizes.

## Masks

```c
#define MASK_4KB  0x000FFFFFFFFFF000UL
#define MASK_2MB  0x000FFFFFFFE00000UL
#define MASK_1GB  0x000FFFFFC0000000UL
```

## Build & Load

```bash
cd demo_06_extract_address
make
sudo insmod addr_driver.ko
cat /proc/demo_addr
sudo rmmod addr_driver
```

## Source

[demo_06_extract_address/](https://github.com/raikrahul/data_cpp/tree/master/demo_06_extract_address)
