---
title: "Demo 03: Read Entry"
driver: entry_driver.c
---

# Demo 03: Read Page Table Entry

Read entries from page tables and calculate addresses.

## Entry Address Formula

```
entry_addr = table_base + (index × 8)
```

Each table: 512 entries × 8 bytes = 4096 bytes

## Build & Load

```bash
cd demo_03_read_entry
make
sudo insmod entry_driver.ko
cat /proc/demo_entry
sudo rmmod entry_driver
```

## Source

[demo_03_read_entry/](https://github.com/raikrahul/data_cpp/tree/master/demo_03_read_entry)
