---
title: "Demo 14: Page Flags"
driver: flags_driver.c
---

# Demo 14: struct page Flags

Display flags from struct page.

## Common Flags

- PG_locked, PG_referenced, PG_uptodate
- PG_dirty, PG_lru, PG_active
- PG_slab, PG_reserved

## Build & Load

```bash
cd demo_14_page_flags
make
sudo insmod flags_driver.ko
cat /proc/demo_flags
sudo rmmod flags_driver
```

## Source

[demo_14_page_flags/](https://github.com/raikrahul/data_cpp/tree/master/demo_14_page_flags)
