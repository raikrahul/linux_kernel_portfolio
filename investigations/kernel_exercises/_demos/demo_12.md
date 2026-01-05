---
title: "Demo 12: Process CR3"
driver: pid_driver.c
---

# Demo 12: Process-specific CR3

Read CR3 for different processes.

## Key Concept

Each process has unique CR3/PML4, but kernel entries (256-511) are shared.

## Build & Load

```bash
cd demo_12_process_cr3
make
sudo insmod pid_driver.ko
cat /proc/demo_pid
sudo rmmod pid_driver
```

## Source

[demo_12_process_cr3/](https://github.com/raikrahul/data_cpp/tree/master/demo_12_process_cr3)
