---
title: "Demo 01: Read CR3 Register"
driver: cr3_driver.c
worksheet: /lecpp/demo_01_read_cr3/worksheet.md
---

# Demo 01: Read CR3 Register

This kernel module reads the CR3 register and displays the PML4 physical address.

## Build & Load

```bash
cd demo_01_read_cr3
make
sudo insmod cr3_driver.ko
cat /proc/demo_cr3
sudo rmmod cr3_driver
```

## Expected Output

```
CR3 = 0x0000000305DEF000
PML4 Physical = 0x0000000305DEF000
PML4 Virtual = 0xFFFF89E005DEF000
```

## Key Concepts

1. CR3 holds PML4 physical address
2. Use `__va()` to convert to kernel virtual address
3. Ring 0 required to read CR3

## Source Code

See [demo_01_read_cr3/cr3_driver.c](https://github.com/raikrahul/data_cpp/tree/master/demo_01_read_cr3)
