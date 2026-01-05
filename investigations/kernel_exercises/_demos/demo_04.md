---
title: "Demo 04: Check Present"
driver: present_driver.c
---

# Demo 04: Check Present Bit and Flags

Decode all flags from a page table entry.

## PTE Flags

| Bit | Name | Meaning |
|-----|------|---------|
| 0 | P | Present |
| 1 | R/W | Read/Write |
| 2 | U/S | User/Supervisor |
| 5 | A | Accessed |
| 6 | D | Dirty |
| 7 | PS | Page Size |
| 63 | NX | No Execute |

## Build & Load

```bash
cd demo_04_check_present
make
sudo insmod present_driver.ko
cat /proc/demo_present
sudo rmmod present_driver
```

## Source

[demo_04_check_present/](https://github.com/raikrahul/data_cpp/tree/master/demo_04_check_present)
