---
title: "Demo 02: Extract Indices"
driver: indices_driver.c
---

# Demo 02: Extract Page Table Indices

Extract PML4, PDPT, PD, PT indices from a virtual address.

## Formula

```c
pml4_idx = (va >> 39) & 0x1FF;  // bits 47:39
pdpt_idx = (va >> 30) & 0x1FF;  // bits 38:30
pd_idx   = (va >> 21) & 0x1FF;  // bits 29:21
pt_idx   = (va >> 12) & 0x1FF;  // bits 20:12
offset   = va & 0xFFF;          // bits 11:0
```

## Build & Load

```bash
cd demo_02_extract_indices
make
sudo insmod indices_driver.ko
cat /proc/demo_indices
sudo rmmod indices_driver
```

## Source

[demo_02_extract_indices/](https://github.com/raikrahul/data_cpp/tree/master/demo_02_extract_indices)
