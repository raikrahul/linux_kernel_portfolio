# KERNEL MEMORY LAYOUT - AXIOMATIC TEXTBOOK
# SPECIFIC TO: r's AMD Ryzen 5 4600H

## MACHINE IDENTITY

```
CPU:     AMD Ryzen 5 4600H with Radeon Graphics
Cores:   6 physical, 12 threads (SMT)
Cache:   512 KB L2 per core
MHz:     1397 (current), ~3000 boost
Kernel:  6.14.0-37-generic (Ubuntu 24.04)
Arch:    x86_64
TLB:     3072 entries × 4KB = 12 MB coverage
PCID:    NOT SUPPORTED (AMD uses ASID for VMs only)
KASLR:   ENABLED (CONFIG_RANDOMIZE_MEMORY=y)
```

## YOUR REAL ADDRESSES (from /proc/self/maps)

```
┌─────────────────────────────────────────────────────────────────────┐
│ REGION           │ START            │ END              │ PERM      │
├─────────────────────────────────────────────────────────────────────┤
│ /usr/bin/cat     │ 0x62912dcce000   │ 0x62912dcd0000   │ r--p      │
│   .text          │ 0x62912dcd0000   │ 0x62912dcd5000   │ r-xp      │
│   .rodata        │ 0x62912dcd5000   │ 0x62912dcd7000   │ r--p      │
│   .data          │ 0x62912dcd8000   │ 0x62912dcd9000   │ rw-p      │
│ [heap]           │ 0x629167041000   │ 0x629167062000   │ rw-p      │
│ libc.so.6        │ 0x7f8fa8200000   │ 0x7f8fa8405000   │ mixed     │
│ ld-linux.so.2    │ 0x7f8fa8440000   │ 0x7f8fa847a000   │ mixed     │
│ [stack]          │ 0x7ffe5e4ed000   │ 0x7ffe5e50e000   │ rw-p      │
│ [vsyscall]       │ 0xffffffffff600000│ 0xffffffffff601000│ --xp     │
└─────────────────────────────────────────────────────────────────────┘

DERIVE PML4 indices from real addresses:

  stack = 0x7ffe5e4ed000
  (0x7ffe5e4ed000 >> 39) & 0x1FF = ?
  
  0x7FFE... → 7 = 0111, F = 1111, F = 1111, E = 1110
  bits [47:39] = 0_1111_1111 = 255
  ∴ stack at PML4[255] (user space, last user slot)
  
  vsyscall = 0xffffffffff600000
  (0xffffffffff600000 >> 39) & 0x1FF = 511
  ∴ vsyscall at PML4[511] (kernel space)
```

## AXIOM 1: YOUR RAM SIZE

```
DATA: MemTotal = 15776276 kB (from /proc/meminfo)

DERIVATION:
  01. 15776276 kB = 15776276 × 1024 bytes
  02. 15776276 × 1024 = ?
      15776276 × 1000 = 15,776,276,000
      15776276 × 24   = 378,630,624
      SUM = 15,776,276,000 + 378,630,624 = 16,154,906,624 bytes
  03. 16,154,906,624 ÷ 4096 = ?
      16,154,906,624 ÷ 4096 = 3,944,069 pages
  04. VERIFY: 3,944,069 × 4096 = 16,154,906,624 ✓
```

## AXIOM 2: PAGE SIZE

```
GIVEN: x86_64 page = 4KB

DERIVATION:
  01. 4KB = 4 × 1024 = 4096 bytes
  02. 4096 = 2^? → 2^12 = 4096 ✓
  03. ∴ 12 bits needed for offset within page
  04. ∴ bits [11:0] = page offset
```

## AXIOM 3: VIRTUAL ADDRESS BIT SPLIT

```
GIVEN: 48-bit virtual address

DERIVATION:
  01. Total bits = 48
  02. Offset = 12 bits (from AXIOM 2)
  03. Remaining = 48 - 12 = 36 bits
  04. 4 page table levels
  05. 36 ÷ 4 = 9 bits per level
  06. VERIFY: 12 + 9 + 9 + 9 + 9 = 48 ✓
  
BIT POSITIONS:
  bits [11:0]  = offset   = 12 bits
  bits [20:12] = PT index = 9 bits
  bits [29:21] = PD index = 9 bits
  bits [38:30] = PDPT idx = 9 bits
  bits [47:39] = PML4 idx = 9 bits
```

## AXIOM 4: KERNEL/USER SPLIT

```
GIVEN: Bit 47 determines kernel vs user

DERIVATION:
  01. PML4 index = bits [47:39] = 9 bits
  02. 9 bits = 2^9 = 512 possible indices [0..511]
  03. bit 47 = MSB of PML4 index
  04. IF bit 47 = 0 → PML4 index = 0_xxxx_xxxx → [0..255]
  05. IF bit 47 = 1 → PML4 index = 1_xxxx_xxxx → [256..511]
  
      VERIFY: 256 binary = 1_0000_0000 (bit 8 set = bit 47 of VA)
      VERIFY: 255 binary = 0_1111_1111 (bit 8 clear)
  
  06. ∴ User space = PML4[0..255]
  07. ∴ Kernel space = PML4[256..511]
```

## AXIOM 5: YOUR CR3 VALUE

```
DATA: CR3 = 0x2ccb6c000 (from your earlier run)

DERIVATION:
  01. CR3 bits [51:12] = PML4 physical address
  02. CR3 bits [11:0] = PCID/flags
  03. Mask = 0x000FFFFFFFFFF000
  04. 0x2ccb6c000 & 0x000FFFFFFFFFF000 = ?
      0x2ccb6c000 ends in 000
      Low 12 bits = 0x000 = 0
      ∴ Result = 0x2ccb6c000
  05. PML4 physical = 0x2ccb6c000
```

## AXIOM 6: PHYSICAL TO VIRTUAL CONVERSION

```
GIVEN: Kernel maps phys RAM to (page_offset_base + phys)

YOUR KERNEL:
  01. KASLR enabled → page_offset_base ≠ 0xFFFF888000000000
  02. page_offset_base stored at 0xffffffff9695b668
  03. Value randomized each boot

EXAMPLE (if base = 0xFFFF888000000000):
  01. phys = 0x2ccb6c000
  02. virt = 0xFFFF888000000000 + 0x2ccb6c000
  
  CALCULATE:
    0xFFFF_8880_0000_0000
  + 0x0000_0002_CCB6_C000
  ─────────────────────────
    Position-by-position (right to left, 4 bits each):
    0 + 0 = 0
    0 + 0 = 0
    0 + 0 = 0
    C + 0 = C
    6 + 0 = 6
    B + 0 = B
    C + 0 = C
    C + 0 = C
    2 + 0 = 2
    0 + 0 = 0
    0 + 8 = 8
    0 + 8 = 8
    0 + 8 = 8
    F + 0 = F
    F + 0 = F
    F + 0 = F
    
  03. Result = 0xFFFF_8882_CCB6_C000
```

## AXIOM 7: YOUR KERNEL TEXT

```
DATA: _text = 0xffffffff94c00000 (from /proc/kallsyms)

CALCULATE PML4 INDEX:
  01. (0xffffffff94c00000 >> 39) & 0x1FF = ?
  
  02. 0xffffffff94c00000 hex to nibbles:
      F F F F F F F F 9 4 C 0 0 0 0 0
      positions 60-63, 56-59, ..., 0-3
      
  03. bits [47:39]:
      bit 47 in nibble at position 44-47 = F = 1111 → bit 47 = 1
      bit 46-44 = 111
      bit 43-40 = F = 1111
      bit 39 = top bit of 9 = 1001 → bit 39 = 1
      
  04. bits [47:39] = 1_1111_1111 = 0x1FF = 511
  
  05. PML4 index = 511
  06. VERIFY: 511 = max PML4 index ✓
      ∴ Kernel text at PML4[511]
```

## EXERCISES - CALCULATE BY HAND

```
EX1: Your RAM in pages
  15776276 kB × 1024 ÷ 4096 = ___
  
EX2: Your CR3 masked
  0x2ccb6c000 & 0x000FFFFFFFFFF000 = ___
  
EX3: 0xFFFF888000000000 + 0x100000 = ___
  (add position by position)
```

## FAILURE PREDICTIONS

```
FAIL 1: Hardcode 0xFFFF888000000000 on KASLR kernel
  Expected: Direct map works
  Actual: PML4[273] = 0 → page fault
  Why: Your kernel randomizes base

FAIL 2: Forget CR3 mask
  CR3 = 0x12eb28005
  Expected: 0x12eb28000
  Actual: 0x12eb28005 (off by 5 bytes) ✗
  
FAIL 3: Use physical in C pointer
  ptr = (void *)0x12eb28000
  Expected: Read RAM
  Actual: Unmapped virtual → fault ✗
  
FAIL 4: Wrong process CR3
  Walk user VA with cat's page tables
  Expected: User page found
  Actual: NOT PRESENT ✗
```
