```
═══════════════════════════════════════════════════════════════════════════════
DEMO 07: FULL 4KB PAGE WALK - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | RAM=15406 MB | page_offset_base=0xFFFF89DF00000000
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: COMPLETE PAGE WALK FOR VA=0xFFFFFFFF81000000 (kernel text)
─────────────────────────────────────────────────────────────────────────────────

STEP 0: Extract all indices by hand
┌─────────────────────────────────────────────────────────────────────────────┐
│ VA = 0xFFFFFFFF81000000                                                     │
│                                                                             │
│ PML4 = (0xFFFFFFFF81000000 >> 39) & 0x1FF = _____ (calculate)              │
│                                                                             │
│ Work: 0xFFFFFFFF81000000 >> 39                                             │
│       Hex: FFFFFFFF81 (drop last 10 hex digits = 40 bits)                  │
│       Wait, 39 bits = 9 hex + 3 bits. Complex.                             │
│                                                                             │
│       Binary approach:                                                      │
│       F = 1111, F = 1111, F = 1111, F = 1111, F = 1111,                    │
│       F = 1111, F = 1111, F = 1111, 8 = 1000, 1 = 0001, ...                │
│                                                                             │
│       Bits [47:39]:                                                         │
│       Let's find hex positions:                                             │
│       Hex 11 (bits 47-44) = F = 1111                                       │
│       Hex 10 (bits 43-40) = F = 1111                                       │
│       Hex 9  (bits 39-36) = 8 = 1000                                       │
│                                                                             │
│       Bits 47,46,45,44 = 1,1,1,1                                           │
│       Bits 43,42,41,40 = 1,1,1,1                                           │
│       Bit 39 = 1 (MSB of 8)                                                │
│       = 1,1,1,1,1,1,1,1,1 = 0x1FF = 511                                   │
│                                                                             │
│ Hmm wait. Let me reconsider. 0xFFFFFFFF81000000:                           │
│ Hex digits:  F F F F F F F F 8 1 0 0 0 0 0 0                               │
│ Position:   15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0                          │
│                                                                             │
│ Hex 11 = F, Hex 10 = F, Hex 9 = 8, Hex 8 = 1                               │
│ Bits 47-44 (hex 11) = F = 1111                                             │
│ Bits 43-40 (hex 10) = F = 1111                                             │
│ Bits 39-36 (hex 9)  = 8 = 1000                                             │
│                                                                             │
│ PML4 [47:39] = bits: 1111 1111 1 = 9 bits = 0x1FF = 511                   │
│ WAIT: That's 9 ones, but hex 9 = 8 = 1000, so bit 39 = 1                   │
│ So bits [47:39] = 1,1,1,1,1,1,1,1,1 = 511? Yes.                            │
│                                                                             │
│ But wait, 0xFFFFFFFF81000000 is in kernel. PML4[511] is highest entry.     │
│ Seems wrong. Let me verify differently.                                    │
│                                                                             │
│ Actually for kernel start 0xFFFFFFFF81000000:                              │
│ This is _text address on x86_64 Linux.                                     │
│ Should map to PML4[511] typically.                                         │
│                                                                             │
│ PDPT = (VA >> 30) & 0x1FF                                                  │
│ Bits [38:30]:                                                               │
│ Hex 9 low 3 bits (38,37,36) + hex 8 (35-32) + hex 7 upper 2 (31,30)       │
│ 8 = 1000, bits 39-36 → bits 38,37,36 = 0,0,0                               │
│ 1 = 0001, bits 35-32 → 0,0,0,1                                             │
│ 0 = 0000, bits 31-28 → 0,0,0,0                                             │
│                                                                             │
│ Bits [38:30] = 0,0,0,0,0,0,1,0,0 = 0b000000100 = 4                         │
│                                                                             │
│ PD = (VA >> 21) & 0x1FF                                                    │
│ Similar analysis... bits [29:21]                                           │
│ hex 5 (23-20), part of hex 6 (27-24)                                       │
│ All zeros in 0x...81000000 below position 8                                │
│ 8 = position 7 = bits 31-28 = 1000                                         │
│ 1 = position 6 = bits 27-24 = 0001                                         │
│ Rest all 0                                                                  │
│                                                                             │
│ Bits [29:21] = 0 (all zeros there) → PD = 0? Let me verify.               │
│ 0x81000000 >> 21 = 0x408                                                   │
│ 0x408 & 0x1FF = 0x008 = 8                                                  │
│ So PD = 8.                                                                  │
│                                                                             │
│ PT = (VA >> 12) & 0x1FF                                                    │
│ 0x81000000 >> 12 = 0x81000                                                 │
│ 0x81000 & 0x1FF = 0x000 = 0                                                │
│                                                                             │
│ OFF = VA & 0xFFF = 0                                                       │
│                                                                             │
│ FINAL: PML4=511, PDPT=4, PD=8, PT=0, OFF=0                                 │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 1: L4 (PML4) Read
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: CR3 = 0x_______________ (fill from your system)                      │
│ Mask: CR3 & 0x000FFFFFFFFFF000 = 0x_______________ = PML4_phys             │
│ Convert: __va(PML4_phys) = page_offset_base + PML4_phys                    │
│        = 0xFFFF89DF00000000 + 0x_______________ = 0x_______________        │
│                                                                             │
│ Read: PML4[511] at offset 511 × 8 = 4088 = 0xFF8                          │
│ Entry address = PML4_virt + 0xFF8 = 0x_______________                      │
│ Entry value = *(0x_______________) = 0x_______________                     │
│                                                                             │
│ Check Present: entry & 1 = _____ (0 or 1?)                                 │
│ If 0 → STOP (not present)                                                  │
│ If 1 → Extract PDPT_phys = entry & 0x000FFFFFFFFFF000 = 0x_______________  │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 2: L3 (PDPT) Read
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: PDPT_phys from Step 1                                                │
│ Convert: PDPT_virt = __va(PDPT_phys) = 0x_______________                   │
│                                                                             │
│ Read: PDPT[4] at offset 4 × 8 = 32 = 0x20                                  │
│ Entry address = PDPT_virt + 0x20 = 0x_______________                       │
│ Entry value = 0x_______________                                             │
│                                                                             │
│ Check Present: entry & 1 = _____                                           │
│ Check Huge (PS): (entry >> 7) & 1 = _____                                  │
│                                                                             │
│ If PS=1 → 1GB huge page, STOP here                                         │
│   Physical = (entry & 0x000FFFFFC0000000) | (VA & 0x3FFFFFFF)              │
│                                                                             │
│ If PS=0 → Continue, extract PD_phys = entry & MASK = 0x_______________     │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 3: L2 (PD) Read
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: PD_phys from Step 2                                                  │
│ Convert: PD_virt = __va(PD_phys) = 0x_______________                       │
│                                                                             │
│ Read: PD[8] at offset 8 × 8 = 64 = 0x40                                    │
│ Entry address = PD_virt + 0x40 = 0x_______________                         │
│ Entry value = 0x_______________                                             │
│                                                                             │
│ Check Present: entry & 1 = _____                                           │
│ Check Huge (PS): (entry >> 7) & 1 = _____                                  │
│                                                                             │
│ If PS=1 → 2MB huge page, STOP here                                         │
│   Physical = (entry & 0x000FFFFFFFE00000) | (VA & 0x1FFFFF)                │
│                                                                             │
│ If PS=0 → Continue, extract PT_phys = entry & MASK = 0x_______________     │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 4: L1 (PT) Read
┌─────────────────────────────────────────────────────────────────────────────┐
│ Input: PT_phys from Step 3                                                  │
│ Convert: PT_virt = __va(PT_phys) = 0x_______________                       │
│                                                                             │
│ Read: PT[0] at offset 0 × 8 = 0                                            │
│ Entry address = PT_virt + 0x0 = 0x_______________                          │
│ Entry value = 0x_______________                                             │
│                                                                             │
│ Check Present: entry & 1 = _____                                           │
│ PS bit at L1 is reserved, ignore                                           │
│                                                                             │
│ Extract Page_phys = entry & 0x000FFFFFFFFFF000 = 0x_______________         │
└─────────────────────────────────────────────────────────────────────────────┘

STEP 5: Final Physical Address
┌─────────────────────────────────────────────────────────────────────────────┐
│ Page_phys = 0x_______________ (from Step 4)                                 │
│ Offset = 0x000 = 0                                                          │
│                                                                             │
│ Final_phys = Page_phys | Offset = 0x_______________ | 0x000                │
│            = 0x_______________                                              │
│                                                                             │
│ VERIFICATION:                                                               │
│ __va(Final_phys) should point to kernel text                               │
│ Can compare: *(unsigned long *)__va(Final_phys) should be instruction bytes│
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: DRAW THE WALK AS A CHAIN
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│                                                                             │
│ CR3=0x_________                                                             │
│       │                                                                     │
│       ▼                                                                     │
│ ┌─────────────────────────────────────────────────────────────────────┐    │
│ │ PML4 @ 0x_________ (phys) = 0x_________________ (virt)              │    │
│ │ [0]: 0x_________________ [1]: 0x_________________ ...               │    │
│ │ ...                                                                  │    │
│ │ [511]: 0x_________________ ← READ THIS                               │    │
│ └───────────────────────────────────────────────────────────────┬─────┘    │
│                                                                 │          │
│                                                                 ▼          │
│ ┌─────────────────────────────────────────────────────────────────────┐    │
│ │ PDPT @ 0x_________ (phys) = 0x_________________ (virt)              │    │
│ │ [4]: 0x_________________ ← READ THIS (PS=___?)                       │    │
│ └───────────────────────────────────────────────────────────────┬─────┘    │
│                                                                 │          │
│                                                                 ▼          │
│ ┌─────────────────────────────────────────────────────────────────────┐    │
│ │ PD @ 0x_________ (phys) = 0x_________________ (virt)                │    │
│ │ [8]: 0x_________________ ← READ THIS (PS=___?)                       │    │
│ └───────────────────────────────────────────────────────────────┬─────┘    │
│                                                                 │          │
│                                                                 ▼          │
│ ┌─────────────────────────────────────────────────────────────────────┐    │
│ │ PT @ 0x_________ (phys) = 0x_________________ (virt)                │    │
│ │ [0]: 0x_________________ ← READ THIS                                 │    │
│ └───────────────────────────────────────────────────────────────┬─────┘    │
│                                                                 │          │
│                                                                 ▼          │
│ ┌─────────────────────────────────────────────────────────────────────┐    │
│ │ PAGE @ 0x_________ (phys) = 0x_________________ (virt)              │    │
│ │ Byte[0] = 0x___ (kernel text instruction byte)                       │    │
│ └─────────────────────────────────────────────────────────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: COUNT THE MEMORY READS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ READ 1: PML4[511] = 8 bytes from 0x_______________                         │
│ READ 2: PDPT[4]   = 8 bytes from 0x_______________                         │
│ READ 3: PD[8]     = 8 bytes from 0x_______________                         │
│ READ 4: PT[0]     = 8 bytes from 0x_______________                         │
│ READ 5: Data      = ? bytes from 0x_______________                         │
│                                                                             │
│ Total for translation: 4 × 8 = 32 bytes                                    │
│ If each read = 100ns (DRAM latency), translation cost = 400ns              │
│ With TLB hit: ~1ns (cached translation)                                    │
│ Speedup: 400× faster with TLB!                                             │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Forgot to check P bit → dereferenced garbage pointer → crash
F2. Ignored PS=1 at L3/L2 → continued walk past huge page → wrong address
F3. Used physical address directly → page fault (CPU sees it as virtual)
F4. Index calculation error → read wrong table entry → wrong path
F5. Wrong mask for huge page → lost offset bits → wrong final address
F6. Did not __va() before dereference → crash
```
