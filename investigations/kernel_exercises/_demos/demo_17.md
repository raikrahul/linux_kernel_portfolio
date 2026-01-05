---
title: "Demo 17: Buddy Info"
driver: buddy_driver.c
---

# Demo 17: Buddy Allocator Info

Display buddy allocator free lists.

## Orders

Order 0 = 4KB, Order 11 = 8MB (max)

## Build & Load

```bash
cd demo_17_buddy_info
make
sudo insmod buddy_driver.ko
cat /proc/demo_buddy
sudo rmmod buddy_driver
```

## Source

[demo_17_buddy_info/](https://github.com/raikrahul/data_cpp/tree/master/demo_17_buddy_info)
