```
═══════════════════════════════════════════════════════════════════════════════
DEMO 17: BUDDY ALLOCATOR - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | RAM=15406 MB | MAX_ORDER typically 11
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: ORDER TO PAGE COUNT CONVERSION
─────────────────────────────────────────────────────────────────────────────────

FORMULA: pages = 2^order

┌─────────────────────────────────────────────────────────────────────────────┐
│ Order │ Pages │ Size in Pages │ Size in Bytes │ Size (Human)              │
├───────┼───────┼───────────────┼───────────────┼───────────────────────────│
│ 0     │ 2^0   │ 1 page        │ 4096          │ 4 KB                      │
│ 1     │ 2^1   │ 2 pages       │ 8192          │ 8 KB                      │
│ 2     │ 2^2   │ 4 pages       │ 16384         │ 16 KB                     │
│ 3     │ 2^3   │ 8 pages       │ 32768         │ 32 KB                     │
│ 4     │ 2^4   │ 16 pages      │ 65536         │ 64 KB                     │
│ 5     │ 2^5   │ 32 pages      │ 131072        │ 128 KB                    │
│ 6     │ 2^6   │ 64 pages      │ 262144        │ 256 KB                    │
│ 7     │ 2^7   │ 128 pages     │ 524288        │ 512 KB                    │
│ 8     │ 2^8   │ 256 pages     │ 1048576       │ 1 MB                      │
│ 9     │ 2^9   │ 512 pages     │ 2097152       │ 2 MB                      │
│ 10    │ 2^10  │ 1024 pages    │ 4194304       │ 4 MB                      │
│ 11    │ 2^11  │ 2048 pages    │ 8388608       │ 8 MB (typical MAX_ORDER)  │
└───────┴───────┴───────────────┴───────────────┴───────────────────────────┘

CALCULATE BY HAND:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Need 100 KB. What order?                                                   │
│ 100 KB = 100 × 1024 = 102,400 bytes                                        │
│ 102,400 / 4096 = 25 pages                                                  │
│ 2^4 = 16 < 25 < 32 = 2^5                                                   │
│ ∴ Order 5 (32 pages = 128 KB)                                              │
│ Waste = 128 - 100 = 28 KB = 28% overhead                                   │
│                                                                             │
│ Need 5 MB. What order?                                                     │
│ 5 MB = 5 × 1024 × 1024 = 5,242,880 bytes                                   │
│ 5,242,880 / 4096 = 1280 pages                                              │
│ 2^10 = 1024 < 1280 < 2048 = 2^11                                           │
│ ∴ Order 11 (2048 pages = 8 MB)                                             │
│ Waste = 8 - 5 = 3 MB = 37.5% overhead                                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: BUDDY SPLITTING
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Request order-2 (4 pages), but only order-4 (16 pages) available

┌─────────────────────────────────────────────────────────────────────────────┐
│ Initial: Free list has 1 block of order-4 at phys 0x100000                 │
│                                                                             │
│ Order-4 block (16 pages) at 0x100000:                                      │
│ ┌─────────────────────────────────────────────────────────────────────┐   │
│ │  0x100000  0x104000  0x108000  0x10C000  ... 0x13C000               │   │
│ │  Page 0    Page 4    Page 8    Page 12   ... Page 60                │   │
│ └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│ Step 1: Split order-4 into 2× order-3                                     │
│ ┌────────────────────────────┐ ┌────────────────────────────┐             │
│ │ Order-3 block A at 0x100000│ │ Order-3 block B at 0x120000│             │
│ │ 8 pages                    │ │ 8 pages                    │             │
│ └────────────────────────────┘ └────────────────────────────┘             │
│ B goes to order-3 free list                                                │
│                                                                             │
│ Step 2: Split order-3 A into 2× order-2                                   │
│ ┌──────────────┐ ┌──────────────┐                                          │
│ │ Order-2 at   │ │ Order-2 at   │                                          │
│ │ 0x100000     │ │ 0x110000     │                                          │
│ │ 4 pages      │ │ 4 pages      │                                          │
│ └──────────────┘ └──────────────┘                                          │
│ Second order-2 goes to order-2 free list                                   │
│                                                                             │
│ Step 3: Return first order-2 block at 0x100000                            │
│                                                                             │
│ Final state:                                                                │
│   Order-2 free list: 0x110000 (4 pages)                                    │
│   Order-3 free list: 0x120000 (8 pages)                                    │
│   Returned: 0x100000 (4 pages)                                             │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: BUDDY COALESCING
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Free order-2 block at 0x100000, buddy at 0x110000 is also free

┌─────────────────────────────────────────────────────────────────────────────┐
│ BUDDY ADDRESS CALCULATION:                                                 │
│ buddy_addr = block_addr XOR (block_size)                                   │
│            = 0x100000 XOR (4 × 4096)                                       │
│            = 0x100000 XOR 0x10000                                          │
│            = 0x110000                                                       │
│                                                                             │
│ XOR trick: Toggles the bit that distinguishes the two buddies              │
│ 0x100000 = 0b 0001 0000 0000 0000 0000 0000                               │
│ 0x010000 = 0b 0000 0001 0000 0000 0000 0000 (4 pages = 16KB = 0x4000)     │
│                                                                             │
│ Wait, 4 pages = 4 × 4096 = 16384 = 0x4000, not 0x10000                    │
│ Let me recalculate:                                                        │
│ 0x100000 XOR 0x4000 = 0x104000                                             │
│                                                                             │
│ So buddy of 0x100000 (order 2) is at 0x104000                              │
│ If 0x104000 is free, merge:                                                │
│   Combined block at 0x100000, order 3 (8 pages)                            │
│                                                                             │
│ Then check if buddy of new order-3 (at 0x108000) is free                  │
│   0x100000 XOR 0x8000 = 0x108000                                           │
│ If yes, merge to order-4, and so on...                                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: API USAGE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ ALLOCATION:                                                                 │
│ struct page *page = alloc_pages(GFP_KERNEL, order);                        │
│                                                                             │
│ Returns:                                                                    │
│   - Pointer to struct page (head of compound page for order > 0)           │
│   - NULL on failure                                                         │
│                                                                             │
│ To get virtual address:                                                     │
│   void *vaddr = page_address(page);                                        │
│   // or: void *vaddr = __va(page_to_pfn(page) << PAGE_SHIFT);              │
│                                                                             │
│ DEALLOCATION:                                                               │
│ __free_pages(page, order);                                                 │
│                                                                             │
│ TRAP: Order must match! Wrong order → memory corruption                    │
│ TRAP: Double free → kernel crash                                           │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: 100KB needs order 5, 5MB needs order 11
Problem 2: Split creates 2 buddies, recurse until correct order
Problem 3: buddy(0x100000, order=2) = 0x100000 XOR 0x4000 = 0x104000

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Wrong order on free → corrupted free list
F2. Requesting order > MAX_ORDER → allocation fails
F3. Not checking for NULL return → null pointer dereference
F4. Using page after free → use-after-free
F5. Forgetting GFP_KERNEL can sleep → deadlock in atomic context
```
