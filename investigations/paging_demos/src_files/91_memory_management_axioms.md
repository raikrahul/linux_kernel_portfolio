# MEMORY MANAGEMENT AXIOMS - REAL MACHINE DATA

## MACHINE SPECIFICATIONS (YOUR SYSTEM)
```
Kernel: 6.14.0-37-generic
RAM: 15,776,276 KB = 15.04 GB
CPU: AMD Ryzen 5 4600H (6 cores)
Page Size: 4096 bytes (4 KB)
```

---

## REAL DATA FROM /proc/self/maps

```
USER SPACE REGIONS (bits 47:39 < 256):
┌───────────────────────────────────────────────────────────────────────────────────┐
│ START            END              SIZE      PERM  DESCRIPTION                     │
├───────────────────────────────────────────────────────────────────────────────────┤
│ 0x60d5b1792000 - 0x60d5b1794000   8 KB      r--p  /usr/bin/cat (ELF header)       │
│ 0x60d5b1794000 - 0x60d5b1799000   20 KB     r-xp  /usr/bin/cat (.text)            │
│ 0x60d5b1799000 - 0x60d5b179b000   8 KB      r--p  /usr/bin/cat (.rodata)          │
│ 0x60d5b179c000 - 0x60d5b179d000   4 KB      rw-p  /usr/bin/cat (.data/.bss)       │
│ 0x60d5bd8a6000 - 0x60d5bd8c7000   132 KB    rw-p  [heap]                          │
│ 0x79ca5fe00000 - 0x79ca60005000   ~2 MB     r-xp  libc.so.6                       │
│ 0x7ffe37daf000 - 0x7ffe37dd0000   132 KB    rw-p  [stack]                         │
│ 0xffffffffff600000                4 KB      --xp  [vsyscall]                      │
└───────────────────────────────────────────────────────────────────────────────────┘
```

---

## DRILL: EXTRACT INDICES FROM REAL MAP ADDRESSES

### EXAMPLE 1: cat binary .text section
```
VA = 0x60d5b1794000

01. Convert to binary (48 bits, ignore top 16):
    0x60d5b1794000 = 0110 0000 1101 0101 1011 0001 0111 1001 0100 0000 0000 0000

02. Split: [47:39] [38:30] [29:21] [20:12] [11:0]
    Bits [47:39] = 0_1100_0001 = 0x0C1 = 193
    Bits [38:30] = 1_0101_0110 = 0x156 = 342
    Bits [29:21] = 1_1000_1011 = 0x18B = 395
    Bits [20:12] = 1_1001_0100 = 0x194 = 404
    Bits [11:0]  = 0x000 = 0

03. INDICES:
    PML4  = 193
    PDPT  = 342
    PD    = 395
    PT    = 404
    OFFSET = 0

04. VERIFY: User space? 193 < 256 ✓
```

### EXAMPLE 2: [stack] region
```
VA = 0x7ffe37daf000

01. Hex to binary (48 bits):
    0x7ffe37daf000
    7=0111, f=1111, f=1111, e=1110, 3=0011, 7=0111, d=1101, a=1010, f=1111, 0=0000, 0=0000, 0=0000

02. Binary: 0111 1111 1111 1110 0011 0111 1101 1010 1111 0000 0000 0000

03. Extract [47:39]: 0_1111_1111 = 0xFF = 255
04. Extract [38:30]: 1_1110_0011 = 0x1E3 = 483
05. Extract [29:21]: 0_1111_1101 = 0x0FD = 253
06. Extract [20:12]: 0_1010_1111 = 0x0AF = 175
07. Extract [11:0]:  0x000 = 0

08. INDICES:
    PML4  = 255  ← LAST user entry!
    PDPT  = 483
    PD    = 253
    PT    = 175
    OFFSET = 0

09. EDGE CASE: PML4=255 is boundary. PML4=256+ is kernel.
```

### EXAMPLE 3: [heap] region
```
VA = 0x60d5bd8a6000

01. (0x60d5bd8a6000 >> 39) & 0x1FF = ?

02. 0x60d5bd8a6000 >> 39:
    = 0x60d5bd8a6000 / 2^39
    = 0x60d5bd8a6000 / 549,755,813,888
    = 107,058,306,785,280 / 549,755,813,888
    = 194.7...
    Truncate: 194

03. 194 & 0x1FF = 194 & 511 = 194

04. PML4 Index = 194 ✓ (User space, < 256)
```

---

## DRILL: POINTER ARITHMETIC IN PAGE TABLES

### TABLE ENTRY ADDRESS CALCULATION
```
GIVEN:
  PML4 Base (from CR3) = 0x24fb27000
  PML4 Index = 255
  Entry Size = 8 bytes

CALCULATE:
  Entry Offset = 255 × 8 = 2040 bytes = 0x7F8
  Entry Address = 0x24fb27000 + 0x7F8 = 0x24fb277F8

VERIFY:
  0x7F8 < 0x1000 ✓ (within 4KB page)
  0x24fb277F8 is 8-byte aligned (0x7F8 & 0x7 = 0) ✓
```

### HARDER: MULTI-LEVEL WALK
```
GIVEN (from your pagewalk_driver.c comments):
  CR3 = 0x24fb27000
  VA = 0xffff8a5d40b95400
  PML4 Index = 276

STEP 1: PML4 Entry Address
  = 0x24fb27000 + (276 × 8)
  = 0x24fb27000 + 2208
  = 0x24fb27000 + 0x8A0
  = 0x24fb278A0

STEP 2: Read PML4[276]
  RAM[0x24fb278A0] = 0x393c01067

STEP 3: Extract PDPT Base
  0x393c01067 & 0x000F_FFFF_FFFF_F000 = 0x393c01000
  (Mask clears bits 11:0 and bits 63:52)

STEP 4: PDPT Entry Address
  PDPT Index = 373
  = 0x393c01000 + (373 × 8)
  = 0x393c01000 + 2984
  = 0x393c01000 + 0xBA8
  = 0x393c01BA8

...continue for PD, PT...
```

---

## DRILL: PTE FLAG DECODING

### REAL PTE FROM YOUR DRIVER
```
PTE = 0x8000000100b95163

BINARY OF LOW BYTE (0x63):
  0x63 = 0110 0011
         ↑↑↑↑ ↑↑↑↑
         7654 3210  (bit positions)

BIT-BY-BIT:
  Bit 0 (Present)    = 1 → Page in RAM ✓
  Bit 1 (R/W)        = 1 → Writable ✓
  Bit 2 (U/S)        = 0 → Supervisor only (kernel page)
  Bit 3 (PWT)        = 0 → Write-back caching
  Bit 4 (PCD)        = 0 → Caching enabled
  Bit 5 (Accessed)   = 1 → Page was read ✓
  Bit 6 (Dirty)      = 1 → Page was written ✓
  Bit 7 (PS)         = 0 → 4KB page (not huge)

HIGH BIT (0x80...):
  Bit 63 = 1 → NX (No Execute) ✓

PFN EXTRACTION:
  (0x8000000100b95163 >> 12) & 0xF_FFFF_FFFF = 0x100b95
  Physical Page Base = 0x100b95 << 12 = 0x100b95000
```

---

## NUMERICAL PUZZLES (DO BY HAND)

### PUZZLE 1: Index from Address
```
VA = 0xFFFF_8800_1234_5678
Calculate: PML4 Index, PDPT Index, PD Index, PT Index, Offset

STEPS:
1. (VA >> 39) & 0x1FF = PML4
2. (VA >> 30) & 0x1FF = PDPT
3. (VA >> 21) & 0x1FF = PD
4. (VA >> 12) & 0x1FF = PT
5. VA & 0xFFF = Offset

YOUR WORK:
PML4 = ?
PDPT = ?
PD = ?
PT = ?
OFFSET = ?
```

### PUZZLE 2: Entry Address from Base + Index
```
Base = 0x1000_0000
Index = 511 (maximum)
Entry Size = 8 bytes

Entry Address = Base + (Index × Entry_Size) = ?
Is this address still within the 4KB page? (Check < Base + 0x1000)
```

### PUZZLE 3: Is PTE Present?
```
PTE Value = 0x0000_0000_0000_0000

Present? = PTE & 1 = ?
What happens on access? → ?
```

### PUZZLE 4: Huge Page Detection
```
PD Entry = 0x0000_0001_8567_0083

Bit 7 = (0x83 >> 7) & 1 = ?
If Bit 7 = 1 → 2MB Huge Page
Physical Base = Entry & 0xFFFF_FFFF_FFE0_0000 = ?
```

---

## MEMINFO ANALYSIS (YOUR MACHINE)

```
MemTotal:       15,776,276 KB = 15.04 GB
MemFree:        5,309,504 KB  = 5.06 GB
Cached:         3,633,340 KB  = 3.46 GB
Active:         6,957,056 KB  = 6.63 GB

CALCULATIONS:
1. Total Pages = 15,776,276 KB / 4 KB = 3,944,069 pages
2. Free Pages = 5,309,504 KB / 4 KB = 1,327,376 pages
3. Usage Ratio = (15.04 - 5.06) / 15.04 = 66.4% used
4. Cache Ratio = 3.46 / 15.04 = 23% of RAM is cache

PAGE FRAME NUMBER (PFN) RANGE:
  First PFN = 0x0
  Last PFN = Total Pages - 1 = 3,944,068 = 0x3C2F44
  Max Physical Address = 0x3C2F44 << 12 = 0x3C2F44000 = ~15.05 GB ✓
```

---

## FAILURE PREDICTIONS FOR YOUR MACHINE

:01. Swap Never Triggered
    SwapFree = SwapTotal = 4,194,300 KB
    → System has never swapped. RAM is sufficient.
    → If RAM fills, OOM killer activates before swap.

:02. Active(anon) = 4,580,088 KB (4.37 GB)
    → Anonymous memory (malloc, stack) = 29% of RAM
    → No file backing. Must swap on pressure.

:03. Dirty Pages = 724 KB
    → 724 KB / 4 KB = 181 dirty pages
    → If power loss: 181 pages of data lost.

:04. Mlocked = 120 KB
    → 30 pages locked in RAM (cannot swap)
    → Likely: cryptographic keys, GPU buffers

---

## KERNEL MODULE TEST COMMANDS

```bash
# Load module
cd /home/r/Desktop/lecpp/kernel_module
sudo insmod pagewalk_driver.ko

# Test with stack address
echo "stack" | sudo tee /proc/pagewalk
cat /proc/pagewalk

# Test with self (current task_struct)
echo "self" | sudo tee /proc/pagewalk
cat /proc/pagewalk

# Test with specific user-space address
echo "0x7ffe37daf000" | sudo tee /proc/pagewalk
cat /proc/pagewalk

# Unload module
sudo rmmod pagewalk_driver

# View kernel log
dmesg | tail -50
```

---

## EXERCISES USING YOUR REAL ADDRESSES

### EX1: Walk the heap address
```
Heap Start = 0x60d5bd8a6000
Expected: PML4 index < 256 (user space)
Load module, run: echo "0x60d5bd8a6000" > /proc/pagewalk
Record: PML4 entry value, PDPT entry value, etc.
```

### EX2: Walk the stack address
```
Stack Bottom = 0x7ffe37daf000
Expected: PML4 index = 255 (last user entry)
Verify: PT entry has U/S bit = 1 (user accessible)
```

### EX3: Walk a kernel address (must be root)
```
Kernel text starts at: 0xffffffff81000000
Expected: PML4 index > 255 (kernel space)
Verify: PT entry has U/S bit = 0 (supervisor only)
```
