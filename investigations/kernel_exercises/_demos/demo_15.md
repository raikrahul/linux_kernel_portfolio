---
title: "Demo 15: VMA Walk"
driver: vma_driver.c
---

# Demo 15: VMA Walk

Walk all VMAs of current process.

## VMA Fields

- vm_start, vm_end: Address range
- vm_flags: Permissions
- vm_file: Backing file

## Build & Load

```bash
cd demo_15_vma_walk
make
sudo insmod vma_driver.ko
cat /proc/demo_vma
sudo rmmod vma_driver
```

## Source

[demo_15_vma_walk/](https://github.com/raikrahul/data_cpp/tree/master/demo_15_vma_walk)
