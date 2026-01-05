---
title: "Copy-on-Write (COW)"
difficulty: Intermediate
order: 6
---

# Copy-on-Write

COW shares pages between parent and child until write, then copies.

## Without COW

```
fork() creates child:
  Parent: 2048 pages = 8 MB
  Child:  Copy all 2048 pages = 8 MB
  Total:  16 MB
  Time:   8 MB × 100 ns/page = 200,000 ns = 0.2 ms
```

## With COW

```
fork() creates child:
  Parent: 2048 pages = 8 MB
  Child:  Share PTEs, mark R/W=0
  Total:  8 MB (shared!)
  Time:   Copy page tables only = ~1000 ns
  
Savings: 8 MB RAM, 200× faster fork
```

## COW Lifecycle

```
1. BEFORE FORK:
   PTE: phys=0x12345000, R/W=1, refcount=1
   
2. AFTER FORK (both processes):
   PTE: phys=0x12345000, R/W=0, refcount=2
   Page marked read-only!
   
3. CHILD WRITES:
   → Page fault (R/W=0)
   → Allocate new page at 0x67890000
   → Copy 4KB data
   → Child PTE: phys=0x67890000, R/W=1
   → Parent PTE: refcount=1, can set R/W=1
   
4. NOW SEPARATE:
   Parent: 0x12345000, R/W=1
   Child:  0x67890000, R/W=1
```

## PTE Bits for COW

```
Before fork:  0x0000000012345067  (R/W=1)
After fork:   0x0000000012345065  (R/W=0)
                              ↑
                              bit 1 = 0
After COW:    0x0000000067890067  (R/W=1, new address)
```
