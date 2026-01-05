---
title: "Demo 07: 4KB Walk"
driver: walk4kb_driver.c
---

# Demo 07: Complete 4KB Page Walk

Full 4-level page table walk for 4KB pages.

## Walk Steps

```
CR3 → PML4[idx] → PDPT[idx] → PD[idx] → PT[idx] → Page
```

## Build & Load

```bash
cd demo_07_walk_4kb
make
sudo insmod walk4kb_driver.ko
cat /proc/demo_walk4kb
sudo rmmod walk4kb_driver
```

## Source

[demo_07_walk_4kb/](https://github.com/raikrahul/data_cpp/tree/master/demo_07_walk_4kb)
