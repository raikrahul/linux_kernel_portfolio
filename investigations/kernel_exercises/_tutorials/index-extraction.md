---
title: "Page Table Index Extraction"
difficulty: Beginner
order: 2
---

# Page Table Index Extraction

Extract indices from a 48-bit virtual address for 4-level page table walk.

## VA Bit Layout

```
┌─────────────────────────────────────────────────────────────────┐
│ 63:48   │ 47:39   │ 38:30   │ 29:21  │ 20:12  │ 11:0          │
│ Sign ext│ PML4 idx│ PDPT idx│ PD idx │ PT idx │ Page offset   │
│ 16 bits │ 9 bits  │ 9 bits  │ 9 bits │ 9 bits │ 12 bits       │
└─────────────────────────────────────────────────────────────────┘
```

## Extraction Formulas

```c
pml4_idx = (va >> 39) & 0x1FF;  // bits 47:39
pdpt_idx = (va >> 30) & 0x1FF;  // bits 38:30
pd_idx   = (va >> 21) & 0x1FF;  // bits 29:21
pt_idx   = (va >> 12) & 0x1FF;  // bits 20:12
offset   = va & 0xFFF;          // bits 11:0
```

## Example: VA = 0x7FFE5E4ED123

```
1. pml4_idx = (0x7FFE5E4ED123 >> 39) & 0x1FF
           = 0xFF & 0x1FF = 255

2. pdpt_idx = (0x7FFE5E4ED123 >> 30) & 0x1FF
           = 0x1FF97 & 0x1FF = 407? 
           Actually: 0x1F9 = 505

3. pd_idx = (0x7FFE5E4ED123 >> 21) & 0x1FF
         = 0x2F2 = 242

4. pt_idx = (0x7FFE5E4ED123 >> 12) & 0x1FF
         = 0x0ED = 237

5. offset = 0x123
```

## Each Index Range

- 9 bits → 0-511 (512 entries per table)
- 12 bits → 0-4095 (4KB page offset)
