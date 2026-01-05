---
title: "Demo 10: __va Macro"
driver: va_driver.c
---

# Demo 10: Compare __va Macro

Compare kernel's __va() with manual calculation.

## Formula

```c
#define __va(phys) ((void *)((phys) + page_offset_base))
```

## Build & Load

```bash
cd demo_10_compare_va
make
sudo insmod va_driver.ko
cat /proc/demo_va
sudo rmmod va_driver
```

## Source

[demo_10_compare_va/](https://github.com/raikrahul/data_cpp/tree/master/demo_10_compare_va)
