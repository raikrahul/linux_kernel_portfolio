---
title: "Demo 05: Check Huge"
driver: huge_driver.c
---

# Demo 05: Detect Huge Pages

Check PS bit to identify 2MB and 1GB huge pages.

## PS Bit Location

- L3 (PDPT): PS=1 → 1GB page
- L2 (PD): PS=1 → 2MB page
- L1 (PT): PS not used

## Build & Load

```bash
cd demo_05_check_huge
make
sudo insmod huge_driver.ko
cat /proc/demo_huge
sudo rmmod huge_driver
```

## Source

[demo_05_check_huge/](https://github.com/raikrahul/data_cpp/tree/master/demo_05_check_huge)
