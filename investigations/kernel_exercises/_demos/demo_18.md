---
title: "Demo 18: TLB Flush"
driver: tlb_driver.c
---

# Demo 18: TLB Flush

Demonstrate TLB flush mechanisms.

## Flush Methods

- MOV CR3: Full flush
- INVLPG: Single page flush
- IPI: Cross-CPU shootdown

## Build & Load

```bash
cd demo_18_tlb_info
make
sudo insmod tlb_driver.ko
cat /proc/demo_tlb
sudo rmmod tlb_driver
```

## Source

[demo_18_tlb_info/](https://github.com/raikrahul/data_cpp/tree/master/demo_18_tlb_info)
