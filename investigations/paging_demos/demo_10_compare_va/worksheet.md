```
═══════════════════════════════════════════════════════════════════════════════
DEMO 10: COMPARE __VA MACRO - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | RAM=15406 MB | page_offset_base=0xFFFF89DF00000000
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: DERIVE __VA MACRO
─────────────────────────────────────────────────────────────────────────────────

DEFINITION:
┌─────────────────────────────────────────────────────────────────────────────┐
│ #define __va(phys) ((void *)((phys) + page_offset_base))                   │
│                                                                             │
│ Purpose: Convert physical address to kernel virtual address                │
│ Works for: Addresses in the "direct map" region only                       │
│ Direct map: 1:1 mapping of all physical RAM into kernel space              │
└─────────────────────────────────────────────────────────────────────────────┘

TRIVIAL CASES:
┌─────────────────────────────────────────────────────────────────────────────┐
│ __va(0) = 0 + page_offset_base                                             │
│         = 0 + 0xFFFF89DF00000000                                           │
│         = 0xFFFF89DF00000000 (start of direct map)                         │
│                                                                             │
│ __va(0x1000) = 0x1000 + 0xFFFF89DF00000000                                 │
│              = 0xFFFF89DF00001000                                          │
│                                                                             │
│ __va(0xFFFFFFFF) = 0xFFFFFFFF + 0xFFFF89DF00000000                        │
│                  = 0xFFFF89DEFFFFFFFF (4GB mark)                           │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: INVERSE FUNCTION __PA
─────────────────────────────────────────────────────────────────────────────────

DEFINITION:
┌─────────────────────────────────────────────────────────────────────────────┐
│ #define __pa(virt) ((virt) - page_offset_base)                             │
│                                                                             │
│ Purpose: Convert kernel virtual to physical                                │
│ Works for: Addresses in direct map only                                    │
└─────────────────────────────────────────────────────────────────────────────┘

VERIFY INVERSE:
┌─────────────────────────────────────────────────────────────────────────────┐
│ __pa(__va(X)) = X  ?                                                       │
│                                                                             │
│ __va(0x12345000) = 0x12345000 + 0xFFFF89DF00000000                        │
│                  = 0xFFFF89DF12345000                                      │
│                                                                             │
│ __pa(0xFFFF89DF12345000) = 0xFFFF89DF12345000 - 0xFFFF89DF00000000        │
│                          = 0x12345000 ✓                                    │
│                                                                             │
│ Check subtraction:                                                          │
│   0xFFFF89DF12345000                                                        │
│ - 0xFFFF89DF00000000                                                        │
│ ────────────────────                                                        │
│   0x0000000012345000 ✓                                                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: LIMITS OF __VA
─────────────────────────────────────────────────────────────────────────────────

CALCULATE MAX VALID PHYSICAL:
┌─────────────────────────────────────────────────────────────────────────────┐
│ RAM = 15,776,276 KB = 15,776,276 × 1024 bytes                              │
│     = 16,154,906,624 bytes                                                  │
│     = 0x3C3753000 bytes (approximately)                                    │
│                                                                             │
│ Max valid physical ≈ 0x3C3753000                                           │
│                                                                             │
│ __va(0x3C3753000) = 0xFFFF89DF00000000 + 0x3C3753000                       │
│                   = 0xFFFF89E2C3753000                                      │
│                                                                             │
│ Check: Still in kernel space? 0xFFFF... ✓                                  │
│ Check: Below 0xFFFFFFFFFFFFFFFF? ✓                                        │
└─────────────────────────────────────────────────────────────────────────────┘

WHAT IF PHYSICAL > RAM?
┌─────────────────────────────────────────────────────────────────────────────┐
│ __va(0x100000000000) = 0xFFFF89DF00000000 + 0x100000000000                 │
│                      = 0xFFFF99DF00000000                                   │
│                                                                             │
│ This is a valid virtual address, but:                                      │
│ - No physical RAM at 0x100000000000 (16 TB)                                │
│ - Direct map only covers actual RAM                                        │
│ - Accessing this VA → page fault or garbage                                │
│                                                                             │
│ TRAP: __va() does NO validation! It's just arithmetic.                     │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: KASLR AND page_offset_base
─────────────────────────────────────────────────────────────────────────────────

KASLR = Kernel Address Space Layout Randomization

┌─────────────────────────────────────────────────────────────────────────────┐
│ Without KASLR:                                                             │
│   page_offset_base = 0xFFFF888000000000 (fixed)                            │
│   Attacker knows exact kernel addresses                                    │
│                                                                             │
│ With KASLR:                                                                │
│   page_offset_base = 0xFFFF89DF00000000 (this boot)                        │
│   page_offset_base = 0xFFFF8A2300000000 (next boot)                        │
│   Different each boot, random offset                                       │
│                                                                             │
│ To read page_offset_base in module:                                        │
│   extern unsigned long page_offset_base;                                   │
│   printk("base = 0x%lx\n", page_offset_base);                              │
│                                                                             │
│ TRAP: Hardcoding page_offset_base → works once, breaks on reboot          │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: MANUAL vs MACRO COMPARISON
─────────────────────────────────────────────────────────────────────────────────

Given: phys = 0x2FAB7000

MACRO:
┌─────────────────────────────────────────────────────────────────────────────┐
│ virt = __va(0x2FAB7000);                                                   │
│ Compiler generates: ADD RAX, [page_offset_base]                            │
│ Or with constant: ADD RAX, 0xFFFF89DF00000000                              │
│ Result: 0xFFFF89E00FAB7000                                                 │
│                                                                             │
│ Wait, let me calculate properly:                                           │
│   0xFFFF89DF00000000                                                        │
│ + 0x000000002FAB7000                                                        │
│ ────────────────────                                                        │
│                                                                             │
│ Low: 0x00000000 + 0x2FAB7000 = 0x2FAB7000                                  │
│ High: 0x89DF + 0x0000 = 0x89DF (no carry)                                  │
│ Result = 0xFFFF89DF2FAB7000                                                │
└─────────────────────────────────────────────────────────────────────────────┘

MANUAL (if you didn't know about page_offset_base):
┌─────────────────────────────────────────────────────────────────────────────┐
│ Walk page tables to find the mapping?                                      │
│ That's circular: need __va() to read page tables!                          │
│                                                                             │
│ Kernel sets up direct map at boot:                                         │
│ 1. Before paging enabled: identity map (phys=virt)                         │
│ 2. Create page tables mapping all RAM at page_offset_base                  │
│ 3. Enable paging                                                            │
│ 4. Now __va() works for any physical address in RAM                        │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: __va(0) = 0xFFFF89DF00000000
Problem 2: __pa(__va(0x12345000)) = 0x12345000
Problem 3: Max valid ≈ 0x3C3753000
Problem 5: __va(0x2FAB7000) = 0xFFFF89DF2FAB7000

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Using __va() on address > RAM size → valid VA but unmapped
F2. Using __va() on MMIO addresses → wrong, need ioremap()
F3. Hardcoding page_offset_base → breaks with KASLR / different boot
F4. Confusing __va() with phys_to_virt() (they're same on x86)
F5. Using __pa() on non-direct-map address → garbage result
```
