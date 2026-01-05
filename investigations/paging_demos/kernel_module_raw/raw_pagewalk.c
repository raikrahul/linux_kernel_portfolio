/*
 * RAW PAGE TABLE WALKER - NO ABSTRACTIONS
 * ========================================
 *
 * BLOCK 01: INCLUDES
 * ------------------
 * FILL THESE VALUES BEFORE WRITING CODE:
 *
 * Q01: What is the physical address format on x86_64?
 *      Answer: ___ bits (fill in)
 *
 * Q02: What header defines unsigned long on Linux kernel?
 *      Answer: linux/_______.h (fill in)
 *
 * Q03: How many bytes is unsigned long on x86_64?
 *      Answer: ___ bytes (fill in)
 */

#include <asm/io.h>
#include <linux/kernel.h> /* Q05: What macro does this provide? printk uses KERN_____ */
#include <linux/mm.h>
#include <linux/module.h>  /* Q04: What function does this provide? module_____() */
#include <linux/proc_fs.h> /* Q06: proc_create returns pointer to struct _______ */
MODULE_LICENSE("GPL");

extern unsigned long page_offset_base;

/*
 * BLOCK 02: RAW NUMBERS - NO MACROS
 * ---------------------------------
 *
 * DERIVE EACH NUMBER FROM SCRATCH:
 *
 * Q07: Page size = 4096 bytes = 2^12 bytes
 *      Fill: PAGE_SIZE_BITS = 12
 *
 * Q08: Each page table has 512 entries. 512 = 2^9
 *      Fill: ENTRIES_PER_TABLE_BITS = 9
 *
 * Q09: Virtual address is 48 bits used. 48 = 12 + 9 + 9 + 9 + 9
 *      Fill: The missing level bits = 9
 *
 * Q10: Entry size = 8 bytes = 2^3 bytes
 *      Fill: ENTRY_SIZE_BITS = 3
 */

/* DO NOT USE MACROS. WRITE RAW NUMBERS. */
static const unsigned long PAGE_SIZE_RAW = 4096;
static const unsigned long ENTRIES_PER_TABLE_RAW = 512;
static const unsigned long ENTRY_SIZE_RAW = 8;

/* TODO: FILL AFTER ANSWERING Q07-Q10 */

/*
 * BLOCK 03: MASK DERIVATION
 * -------------------------
 *
 * PROBLEM: Extract 9 bits from position [47:39] of a 64-bit number.
 *
 * STEP 3.1: What is the mask to keep only 9 bits?
 *           Mask = 2^9 - 1 = _512__ - 1 = _511_
 *           In hex: 0x1ff
 *
 * STEP 3.2: What is the shift amount to get bits [47:39] into [8:0]?
 *           Shift = 39 (given). But WHY 39?
 *           Bit 39 is the 39th bit (0-indexed from right).
 *           After >> 39, bit 39 becomes bit 0.
 *           After >> 39, bit 47 becomes bit 8.
 *
 * STEP 3.3: Write the expression:
 *           pml4_index = (vaddr >> 39) & 0x1FF;
 *
 * STEP 3.4: VERIFY with real address:
 *           vaddr = 0xFFFF888012345678
 *           vaddr >> 39 = 0x0000ffff8801230xFFFF888012345678_______________ (calculate by hand)
 *           result & 0x1FF =  273 (decimal)
 */

/* TODO: FILL AFTER STEP 3.1-3.4 */

/*
 * BLOCK 04: CR3 REGISTER READ
 * ---------------------------
 *
 * CHAIN: CPU register CR3 → 64-bit value → bits [51:12] = physical address → bits [11:0] = flags
 *
 * Q11: If CR3 = 0x000000011E273005, what is PML4 physical base?
 *      Step 1: Apply mask 0x000FFFFFFFFFF000
 *      Step 2: 0x000000011E273005 & 0x000FFFFFFFFFF000 → 0x000000011E273000
 *
 * Q12: The low 12 bits (0x005) are:
 *      Bit 0 = ___ (meaning: ___)
 *      Bit 2 = ___ (meaning: ___)
 *
 * Q13: Write inline assembly to read CR3:
 *      asm volatile("mov %%___, %0" : "=r"(cr3));
 *      Fill the register name: ___
 */

static inline unsigned long read_cr3_raw(void) {
    unsigned long cr3;
    /* TODO: FILL AFTER Q13 */
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return cr3;
}

/*
 * BLOCK 05: PHYSICAL TO VIRTUAL CONVERSION
 * ----------------------------------------
 *
 * CHAIN: Physical address → Add kernel direct map base → Virtual address
 *
 * Q14: Kernel direct map base on x86_64 = 0xFFFF888000000000
 *      This is a fixed constant. Kernel maps ALL physical RAM starting here.
 *
 *      If physical = 0x11E273000
 *      Then virtual = 0xFFFF888000000000 + 0x11E273000 = 0x_______________
 *
 * Q15: Write the function:
 *      Input:  phys = 0x11E273000
 *      Output: virt = 0x_______________
 *
 * Q16: What type should the return be? (pointer to 8-byte entries)
 *      Return type: unsigned long *
 *      Why pointer? Because we will read 8-byte values from this address.
 */

// #define KERNEL_DIRECT_MAP_BASE 0xFFFF888000000000UL

static inline unsigned long* phys_to_virt_raw(unsigned long phys) {
    /* TODO: FILL AFTER Q14-Q16 */
    /* return (unsigned long *)(KERNEL_DIRECT_MAP_BASE + ___); */
    return (unsigned long*)__va(phys);
}

/*
 * BLOCK 06: INDEX EXTRACTION - LEVEL BY LEVEL
 * -------------------------------------------
 *
 * GIVEN: vaddr = 0x7FFE37DAF000 (stack address from your /proc/self/maps)
 *
 * STEP 6.1: Convert to binary (48 bits, ignore top 16):
 *           7 = 0111, F = 1111, F = 1111, E = 1110
 *           3 = 0011, 7 = 0111, D = 1101, A = 1010
 *           F = 1111, 0 = 0000, 0 = 0000, 0 = 0000
 *
 *           Binary: 0111 1111 1111 1110 0011 0111 1101 1010 1111 0000 0000 0000
 *                   ↑bit47                                              bit0↑
 *
 * STEP 6.2: Mark bit positions:
 *           Bits [47:39] = 0_1111_1111 = 0x___ = ___ (decimal) ← PML4 index
 *           Bits [38:30] = ?_????_???? = 0x___ = ___ (decimal) ← PDPT index
 *           Bits [29:21] = ?_????_???? = 0x___ = ___ (decimal) ← PD index
 *           Bits [20:12] = ?_????_???? = 0x___ = ___ (decimal) ← PT index
 *           Bits [11:0]  = ???? ???? ???? = 0x___ = ___ (decimal) ← Offset
 *
 * STEP 6.3: Extract using shift and mask:
 *           PML4:  (0x7FFE37DAF000 >> 39) & 0x1FF = ?
 *           PDPT:  (0x7FFE37DAF000 >> 30) & 0x1FF = ?
 *           PD:    (0x7FFE37DAF000 >> 21) & 0x1FF = ?
 *           PT:    (0x7FFE37DAF000 >> 12) & 0x1FF = ?
 *           OFF:   0x7FFE37DAF000 & 0xFFF = ?
 *
 * CALCULATE BY HAND. DO NOT USE CALCULATOR.
 */

static void extract_indices_raw(unsigned long vaddr, unsigned long* pml4_idx,
                                unsigned long* pdpt_idx, unsigned long* pd_idx,
                                unsigned long* pt_idx, unsigned long* offset) {
    /* TODO: FILL AFTER STEP 6.1-6.3 */

    *pml4_idx = (vaddr >> 39) & 0x1ff;
    *pdpt_idx = (vaddr >> 30) & 0x1ff;
    *pd_idx = (vaddr >> 21) & 0x1ff;
    *pt_idx = (vaddr >> 12) & 0x1ff;
    *offset = vaddr & 0xfff;
}

/*
 * BLOCK 07: READ PAGE TABLE ENTRY
 * -------------------------------
 *
 * CHAIN: Table base (physical) → Convert to virtual → Add index×8 → Dereference → Entry value
 *
 * Q17: If PML4 base = 0x11E273000 and PML4 index = 255:
 *      0xFFFF888000000000 + 0x11E273000 → 0xFFFF88811E273000
 *      ∴ 0xFFFF88811E273000
 *      Step 2: Entry offset = 255 × 8 = 2040 bytes = 0x7E0 hex
 *      Step 3: Entry address = Virtual base + Entry offset = 0x_______________
 *      Step 4: Entry value = *(unsigned long *)Entry address = ??? (read from RAM)
 *
 * Q18: Entry value = 0x0000000393C01067
 *      Is present? Bit 0 = 0x067 & 1 = ___ (0 or 1)
 *      Next table physical = Entry & 0x000FFFFFFFFFF000 = 0x_______________
 */

static unsigned long read_entry_raw(unsigned long table_phys, unsigned long index) {
    unsigned long* table_virt = __va(table_phys);
    return table_virt[index];
}

/*
 * BLOCK 08: CHECK PRESENT BIT
 * ---------------------------
 *
 * Q19: Entry = 0x8000000100B95163
 *      Bit 0 = 0x163 & 0x1 = ___1
 *      Is present? (Y/N): ___y
 *
 * Q20: Entry = 0x0000000000000000
 *      Bit 0 = 0x0 & 0x1 = ___0
 *      Is present? (Y/N): ___n
 *
 * Q21: If NOT present, what should happen?
 *      Answer: STOP walk. Return error code 0 or print "NOT PRESENT at level 4"
 */

static int is_present_raw(unsigned long entry) {
    /* TODO: FILL AFTER Q19-Q21 */
    /* return (entry & ___) ? 1 : 0; */
    return (entry & 1) ? 1 : 0;
}

/*
 * BLOCK 09: EXTRACT NEXT TABLE ADDRESS
 * ------------------------------------
 *
 * Q22: Entry = 0x0000000393C01067
 *      Apply mask: 0x0000000393C01067 & 0x000FFFFFFFFFF000
 *      Step 1: Keep bits [51:12], clear bits [63:52] and [11:0]
 *      Step 2: Result = 0x_______________
 *      This is the PHYSICAL address of the next-level table.
 *
 * Q23: Why clear bits [11:0]?
 *      Because those are FLAGS, not address bits.
 *      Page tables are 4KB aligned, so low 12 bits of address are always 0.
 *
 * Q24: Why clear bits [63:52]?
 *      Bits [63] = NX (No Execute)
 *      Bits [62:52] = Reserved or available for OS
 *      Not part of physical address.
 */

#define PTE_ADDR_MASK 0x000FFFFFFFFFF000UL

static unsigned long extract_next_table_phys(unsigned long entry) {
    /* TODO: FILL AFTER Q22-Q24 */
    /* return entry & ___; */
    return entry & PTE_ADDR_MASK;
}

/*
 * BLOCK 10: FULL WALK - ASSEMBLE BLOCKS
 * -------------------------------------
 *
 * TRACE WITH REAL DATA:
 *
 * INPUT: vaddr = 0x7FFE37DAF000, CR3 = 0x11E273000
 *
 * STEP 10.1: Extract indices (from BLOCK 06)
 *            pml4_idx = ___, pdpt_idx = ___, pd_idx = ___, pt_idx = ___, offset = ___
 *
 * STEP 10.2: Read PML4[pml4_idx]
 *            table_phys = CR3 & 0x000FFFFFFFFFF000 = 0x_______________
 *            entry = read_entry_raw(table_phys, pml4_idx) = 0x_______________ (hypothetical)
 *            present? = ___
 *            next_phys = entry & 0x000FFFFFFFFFF000 = 0x_______________
 *
 * STEP 10.3: Read PDPT[pdpt_idx]
 *            table_phys = next_phys from step 10.2 = 0x_______________
 *            entry = read_entry_raw(table_phys, pdpt_idx) = 0x_______________
 *            present? = ___
 *            huge page (1GB)? = (entry >> 7) & 1 = ___ (if 1, STOP here)
 *            next_phys = 0x_______________
 *
 * STEP 10.4: Read PD[pd_idx]
 *            (repeat pattern)
 *            huge page (2MB)? = (entry >> 7) & 1 = ___
 *
 * STEP 10.5: Read PT[pt_idx]
 *            (repeat pattern)
 *            This entry contains the FINAL physical page frame.
 *
 * STEP 10.6: Compute physical address
 *            phys_page_base = PT_entry & 0x000FFFFFFFFFF000 = 0x_______________
 *            phys_addr = phys_page_base | offset = 0x_______________
 */

static unsigned long walk_raw(unsigned long cr3, unsigned long vaddr) {
    unsigned long pml4_idx, pdpt_idx, pd_idx, pt_idx, offset;
    unsigned long table_phys, entry;

    /* TODO: FILL AFTER COMPLETING BLOCKS 03-09 */

    /* Extract indices */
    extract_indices_raw(vaddr, &pml4_idx, &pdpt_idx, &pd_idx, &pt_idx, &offset);

    /* Level 4: PML4 */
    table_phys = cr3 & PTE_ADDR_MASK;
    entry = read_entry_raw(table_phys, pml4_idx);
    if (!is_present_raw(entry)) return 0xDEAD0001; /* Error: L4 not present */

    /* Level 3: PDPT */
    table_phys = extract_next_table_phys(entry);
    entry = read_entry_raw(table_phys, pdpt_idx);
    if (!is_present_raw(entry)) return 0xDEAD0002; /* Error: L3 not present */
    if ((entry >> 7) & 1) {
        // 1GB huge page - TODO: handle separately
        return 0xDEAD1001; /* 1GB huge page not implemented */
    }

    /* Level 2: PD */
    table_phys = extract_next_table_phys(entry);
    entry = read_entry_raw(table_phys, pd_idx);
    if (!is_present_raw(entry)) return 0xDEAD0003; /* Error: L2 not present */
    if ((entry >> 7) & 1) {
        /* 2MB huge page - compute physical with 21-bit offset */
        return (entry & 0x000FFFFFFFE00000UL) | (vaddr & 0x1FFFFF);
    }

    /* Level 1: PT */
    table_phys = extract_next_table_phys(entry);
    entry = read_entry_raw(table_phys, pt_idx);
    if (!is_present_raw(entry)) return 0xDEAD0004; /* Error: L1 not present */

    /* Final physical address: page base + 12-bit offset */
    return (entry & PTE_ADDR_MASK) | offset;
}

/*
 * BLOCK 11: MODULE INIT/EXIT
 * --------------------------
 *
 * Q25: What function name does module_init() expect?
 *      Answer: A function with signature: static int __init _____(void)
 *
 * Q26: What function name does module_exit() expect?
 *      Answer: A function with signature: static void __exit _____(void)
 *
 * Q27: Why __init and __exit?
 *      __init = Code is discarded after module loads (saves RAM)
 *      __exit = Code is not included if module is built-in (saves ROM)
 */

static int __init raw_pagewalk_init(void) {
    unsigned long cr3;
    unsigned long test_vaddr = 0x7FFE37DAF000UL; /* Stack address from your maps */
    unsigned long phys;

    printk(KERN_INFO "raw_pagewalk: Loading...\n");

    /* TODO: Read CR3 and print */
    /* cr3 = read_cr3_raw(); */
    /* printk(KERN_INFO "raw_pagewalk: CR3 = 0x%016lx\n", cr3); */

    /* TODO: Walk the test address */
    /* phys = walk_raw(cr3, test_vaddr); */
    /* printk(KERN_INFO "raw_pagewalk: VA 0x%016lx -> PA 0x%016lx\n", test_vaddr, phys); */

    return 0;
}

static void __exit raw_pagewalk_exit(void) { printk(KERN_INFO "raw_pagewalk: Unloading...\n"); }

module_init(raw_pagewalk_init);
module_exit(raw_pagewalk_exit);
