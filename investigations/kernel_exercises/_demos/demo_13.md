---
title: "Demo 13: Zone Info"
driver: zone_driver.c
---

# Demo 13: Memory Zones

Display ZONE_DMA, ZONE_DMA32, ZONE_NORMAL info.

## Zones

| Zone | Range | Purpose |
|------|-------|---------|
| DMA | 0-16 MB | Legacy ISA DMA |
| DMA32 | 16 MB-4 GB | 32-bit DMA |
| NORMAL | 4 GB+ | General use |

## Build & Load

```bash
cd demo_13_zone_info
make
sudo insmod zone_driver.ko
cat /proc/demo_zone
sudo rmmod zone_driver
```

## Source

[demo_13_zone_info/](https://github.com/raikrahul/data_cpp/tree/master/demo_13_zone_info)
