# MISTAKES REPORT - PAGE TABLE WALK SESSION

## CONFUSION 1: "Why add kernel base?"

YOUR THOUGHT: CR3 physical → dereference directly
REALITY: C pointers = virtual. CPU translates. Physical ≠ valid virtual.
ORTHOGONAL: If physical worked, why create page tables at all?

## CONFUSION 2: "Same base for all processes?"

YOUR THOUGHT: Each process needs different base
REALITY: Physical RAM = one world. base + any_physical → works for all.
ORTHOGONAL: If bases varied, how could kernel access arbitrary physical?

## CONFUSION 3: "Kernel maps future addresses"

YOUR THOUGHT: Kernel predicts future virtual addresses
REALITY: Direct map covers ALL physical RAM. Not specific VAs.
ORTHOGONAL: How predict future? Cover all instead.

## CONFUSION 4: "Hardcoded 0xFFFF888... works"

YOUR THOUGHT: Constant from textbook valid everywhere
REALITY: KASLR randomizes. Your kernel: 0xFFFF89DF00000000.
ORTHOGONAL: Why would security accept known constant?

## CONFUSION 5: "22 vs 40 vs 44 vs 48 vs 52 bits"

YOUR THOUGHT: One number for addresses
REALITY: 
  52 = kernel mask capacity
  48 = virtual address bits
  44 = your CPU physical bits
  40 = CR3 field width
  22 = your current CR3 value
ORTHOGONAL: Different limits serve different purposes.

## CONFUSION 6: "4 KB pages only"

YOUR THOUGHT: Textbooks say 4 KB
REALITY: x86_64 supports 4 KB, 2 MB, 1 GB. Bit 7 decides.
ORTHOGONAL: If only 4 KB, why 512 GB page table overhead?

## CONFUSION 7: "Walk stops = no data access"

YOUR THOUGHT: 4 levels mandatory
REALITY: Bit 7 = 1 → stop early. Entry contains page base. VA bits → offset.
ORTHOGONAL: CPU checks bit 7 at each level. Decision point.

## CONFUSION 8: "Offset 12 bits always"

YOUR THOUGHT: offset = VA & 0xFFF always
REALITY: 
  4 KB page: 12-bit offset
  2 MB page: 21-bit offset
  1 GB page: 30-bit offset
ORTHOGONAL: Larger page = more bits absorbed into offset.

## CONFUSION 9: "Physical = base + offset wrong"

YOUR THOUGHT: Shouldn't add
REALITY: base | offset = base + offset (for aligned addresses)
ORTHOGONAL: Low bits of aligned base = 0. Offset fits. No overlap.

## CONFUSION 10: "User process fills half PML4 = can't address 48 bits"

YOUR THOUGHT: 256 entries = limited addressing
REALITY: 256 entries × 512 GB = 128 TB. More than RAM.
ORTHOGONAL: 128 TB >> 15 GB RAM. Most addresses unmapped.

## PATTERN DETECTED

YOU: Jump to conclusion from partial information
FIX: Derive step by step with real data

YOU: Trust textbook constants
FIX: Fetch data from running machine

YOU: Think one number describes all
FIX: Identify which number for which context

YOU: Assume uniform behavior
FIX: Check for conditional logic (bit 7, KASLR)

## NUMBERS TO MEMORIZE

```
page_offset_base = 0xFFFF89DF00000000 (your boot)
CR3 mask = 0x000FFFFFFFFFF000
PTE mask = 0x000FFFFFFFFFF000
4 KB offset = 0xFFF (12 bits)
2 MB offset = 0x1FFFFF (21 bits)
1 GB offset = 0x3FFFFFFF (30 bits)
Bit 7 = Page Size (0=table, 1=huge page)
Bit 0 = Present (0=fault, 1=valid)
```
