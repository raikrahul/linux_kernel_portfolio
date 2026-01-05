---
title: "Demo 09: 1GB Walk"
driver: walk1gb_driver.c
---

# Demo 09: 1GB Huge Page Walk

2-level walk stopping at PDPT with PS=1.

## Walk Steps

```
CR3 → PML4[idx] → PDPT[idx](PS=1) → 1GB Page
```

## Build & Load

```bash
cd demo_09_walk_1gb
make
sudo insmod walk1gb_driver.ko
cat /proc/demo_walk1gb
sudo rmmod walk1gb_driver
```

## Source

[demo_09_walk_1gb/](https://github.com/raikrahul/data_cpp/tree/master/demo_09_walk_1gb)
