---
title: "Demo 20: Swap Info"
driver: swap_driver.c
---

# Demo 20: Swap Subsystem Info

Display swap usage and configuration.

## Swap Entry

When P=0: bits[63:6]=offset, bits[5:1]=type

## Build & Load

```bash
cd demo_20_swap_info
make
sudo insmod swap_driver.ko
cat /proc/demo_swap
sudo rmmod swap_driver
```

## Source

[demo_20_swap_info/](https://github.com/raikrahul/data_cpp/tree/master/demo_20_swap_info)
