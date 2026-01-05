---
title: "Swap Subsystem"
difficulty: Intermediate
order: 7
---

# Swap Subsystem

Swap extends RAM by storing inactive pages on disk.

## Swap Entry Format

When P=0 in PTE but entry is non-zero:

```
┌──────────────────────────────────────────┐
│ 63:6          │ 5:1       │ 0            │
│ Offset        │ Type      │ P=0          │
│ (58 bits)     │ (5 bits)  │              │
└──────────────────────────────────────────┘

Type: Index into swap_info (0-31 devices)
Offset: Position in swap file
```

## Example

```
PTE = 0x0000001234567802
P = 0x...2 & 1 = 0 (not present)
Type = (0x...2 >> 1) & 0x1F = 1
Offset = 0x0000001234567802 >> 6 = 0x48D159E0
```

## Swap-in Process

```
1. Access VA 0x7FFE5E4ED000
2. PTE has P=0, swap entry present
3. Page fault → kernel handler
4. Extract type=1, offset=0x48D159E0
5. Read from swap device at offset
6. Allocate new physical page
7. Copy 4KB from swap to page
8. Update PTE: P=1, new phys address
9. Resume process
```

## Performance

```
RAM read:     100 ns
SSD read:     50,000 ns (50 μs)
HDD read:     10,000,000 ns (10 ms)

Slowdown:
  SSD: 500×
  HDD: 100,000×
```

## Your System

```
Swap file: /swap.img
Size: 4 GB
Used: ~1.4 GB (33%)
Slots: 4 GB / 4 KB = 1,048,576 slots
```
