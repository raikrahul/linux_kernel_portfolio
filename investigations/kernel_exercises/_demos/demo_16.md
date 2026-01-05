---
title: "Demo 16: Slab Info"
driver: slab_driver.c
---

# Demo 16: Slab Allocator Info

Display slab cache information.

## Key Points

- Object caching for fixed sizes
- ~10× faster than buddy
- See /proc/slabinfo

## Build & Load

```bash
cd demo_16_slab_info
make
sudo insmod slab_driver.ko
cat /proc/demo_slab
sudo rmmod slab_driver
```

## Source

[demo_16_slab_info/](https://github.com/raikrahul/data_cpp/tree/master/demo_16_slab_info)
