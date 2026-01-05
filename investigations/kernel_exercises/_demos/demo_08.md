---
title: "Demo 08: 2MB Walk"
driver: walk2mb_driver.c
---

# Demo 08: 2MB Huge Page Walk

3-level walk stopping at PD with PS=1.

## Walk Steps

```
CR3 → PML4[idx] → PDPT[idx] → PD[idx](PS=1) → 2MB Page
```

## Build & Load

```bash
cd demo_08_walk_2mb
make
sudo insmod walk2mb_driver.ko
cat /proc/demo_walk2mb
sudo rmmod walk2mb_driver
```

## Source

[demo_08_walk_2mb/](https://github.com/raikrahul/data_cpp/tree/master/demo_08_walk_2mb)
