/*
 * mapping_hw.c
 * KERNEL INSPECTOR: Reads page->mapping and decodes bottom 2 bits.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/page-flags.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate");
MODULE_DESCRIPTION("Decodes PAGE_MAPPING_FLAGS from page->mapping pointer");

static int target_pid = 0;
module_param(target_pid, int, 0);

static unsigned long target_va = 0;
module_param(target_va, ulong, 0);

static struct page *get_user_page(int pid_nr, unsigned long addr) {
  struct pid *pid_struct;
  struct task_struct *task;
  struct mm_struct *mm;
  struct page *page;
  int ret;

  pid_struct = find_get_pid(pid_nr);
  if (!pid_struct)
    return NULL;

  task = pid_task(pid_struct, PIDTYPE_PID);
  if (!task) {
    put_pid(pid_struct);
    return NULL;
  }

  mm = task->mm;
  if (!mm) {
    put_pid(pid_struct);
    return NULL;
  }

  ret = get_user_pages_remote(mm, addr, 1, 0, &page, NULL);
  put_pid(pid_struct);

  if (ret < 0)
    return NULL;
  return page;
}

static void hexdump_struct_page(struct page *p) {
  /*
   * TODO 7: CRACK THE 0x800000 MYSTERY (REVERSE ENGINEER BY HAND)
   * -------------------------------------------------------------
   * TASK: Write a brute-force HEX DUMP of the 64 bytes of struct page.
   * WHY:  We saw '0x800000' in mapping. We need to see WHICH BYTE it lives in
   * to map it to a field.
   *
   * AXIOM TABLE (BYTES 0-63):
   * 1. [0x00-0x07] FLAGS        : unsigned long (8 bytes).
   *    -> 0xffffEA00..00 : 0x0000000000000000 (Example)
   *
   * 2. [0x08-0x0F] COMPOUND_HEAD: unsigned long (8 bytes) / LRU.next
   *    -> 0xffffEA00..08 : 0xffffEA00..01 (Tail Bit Set)
   *
   * 3. [0x10-0x17] LRU.PREV     : unsigned long (8 bytes) / filler
   *    -> 0xffffEA00..10 : 0xdead000000000122 (List Poison 2?)
   *
   * 4. [0x18-0x1F] MAPPING      : unsigned long (8 bytes) / anon_vma
   *    -> 0xffffEA00..18 : 0x0000000000800000 (THE MYSTERY VALUE)
   *    -> CALC: 0x800000 = 1000... (Binary)
   *    -> Byte Offset: 0x18 + 0 = 0x18 (Little Endian LSB)?
   *    -> Byte Offset: 0x18 + 2 = 0x1A (0x80 is 3rd byte)?
   *    -> 0x800000 >> 16 = 0x80. So it is Byte 2 of the word.
   *    -> Absolute Offset = 0x18 + 2 = 0x1A = 26 decimal.
   *    -> STRUCT OFFSET: 26 bytes from start of struct page.
   *
   * 5. [0x20-0x27] INDEX        : unsigned long (8 bytes)
   *    -> 0xffffEA00..20 : 0x0000000000000000
   *
   * 6. [0x28-0x2F] PRIVATE      : unsigned long (8 bytes)
   *    -> 0xffffEA00..28 : 0x0000000000000000
   *
   * 7. [0x30-0x37] REFCOUNT     : atomic_t (4) + mapcount (4) ??
   *    -> WARNING: Check mm_types.h for exact overlay.
   *
   * INSTRUCTION:
   * 1. Cast `p` to `unsigned long *`.
   * 2. Loop `i` from 0 to 7 (8 words * 8 bytes = 64 bytes).
   * 3. Printk each word with its OFFSET (e.g. "OFFSET 0x08: Value...").
   * 4. DO NOT USE ARRAY NOTATION if you can avoid it. Use Pointer Arithmetic.
   *    -> `addr = (unsigned long)p + (i * 8)`
   *    -> `val = *(unsigned long *)addr`
   */

  // WRITE YOUR CODE HERE:
  // ...
}

static void inspect_mapping(struct page *page) {
  // Call the dump first to see the raw bits
  hexdump_struct_page(page);

  unsigned long raw_mapping;
  unsigned long flags;
  unsigned long clean_ptr;

  /*
   * TODO 1: READ RAW MAPPING VALUE
   * ------------------------------
   * AXIOM: page->mapping is a pointer (64-bit on x86_64).
   * PROBLEM: We cannot use bitwise & on a pointer type in C.
   * WORK: Cast page->mapping to unsigned long.
   *
   * CALCULATION TRACE:
   * 1. page = 0xfffff7da4cc48000
   * 2. offsetof(struct page, mapping) = 24 (Bytes)
   * 3. Address read = 0xfffff7da4cc48000 + 24 = 0xfffff7da4cc48018
   * 4. Value at Address (Example): 0xffff888123456781
   *    - Upper 62 bits: Real Pointer Data (0xffff888123456780)
   *    - Lower 2 bits: 01 (PAGE_MAPPING_ANON)
   * 5. Cast to ulong preserves all bits.
   */
  raw_mapping = (unsigned long)page->mapping;

  /*
   * TODO 2: EXTRACT FLAGS (LOWER 2 BITS)
   * ------------------------------------
   * AXIOM: PAGE_MAPPING_FLAGS mask covers bits 0 and 1.
   * AXIOM: Mask value = 0x3 (Binary 11).
   * WORK: Perform bitwise AND with 0x3.
   *
   * CALCULATION TRACE:
   * 1. raw_mapping = 0xffff888123456781 (bits...1000001)
   * 2. MASK        = 0x0000000000000003 (bits...0000011)
   * 3. AND Operation:
   *      ...1000001
   *    & ...0000011
   *    ------------
   *      ...0000001 (Result = 1 = PAGE_MAPPING_ANON)
   */
  flags = raw_mapping & 0x3UL;

  /*
   * TODO 3: EXTRACT CLEAN POINTER
   * -----------------------------
   * AXIOM: To get the real address, we must zero out limits [1:0].
   * AXIOM: ~0x3 = ~...000011 = ...111100 (Mask to keep upper bits).
   * WORK: Perform bitwise AND with ~0x3UL.
   *
   * CALCULATION TRACE:
   * 1. raw_mapping = 0xffff888123456781
   * 2. MASK (~3)  = 0xfffffffffffffffc (ends in ...1100)
   * 3. AND Operation:
   *      ...00001 (0x...1)
   *    & ...11100 (0x...C)
   *    ----------
   *      ...00000 (0x...0) -> 0xffff888123456780
   * 4. Result: Valid, 8-byte aligned pointer to anon_vma struct.
   */
  clean_ptr = raw_mapping & ~0x3UL;

  printk(KERN_INFO "MAPPING_DECODE: Page Struct   = %px\n", page);
  printk(KERN_INFO "MAPPING_DECODE: Raw mapping   = 0x%lx\n", raw_mapping);
  printk(KERN_INFO "MAPPING_DECODE: Flags [1:0]   = %lu (Binary: %lu%lu)\n",
         flags, (flags >> 1) & 1, flags & 1);

  printk(KERN_INFO "MAPPING_DECODE: Clean Pointer = 0x%lx\n", clean_ptr);

  /*
   * TODO 4: INTERPRET THE STATE (USER EXERCISE)
   * -------------------------------------------
   * AXIOM: We have 2 bits of flags [1:0]. Max value = 3.
   *
   * TASK: Implement the switch/case logic to identify the state.
   *
   * MATH TABLE (Complete this logic):
   * 1. If bits == 0 (00) -> Pointer is Aligned (8-byte).
   *    Type: struct address_space * (Page Cache).
   *
   * 2. If bits == 1 (01) -> PAGE_MAPPING_ANON (0x1).
   *    Type: struct anon_vma *.
   *    Note: "However the lower 2 bits are given over..." (Your Quote).
   *
   * 3. If bits == 2 (10) -> PAGE_MAPPING_MOVABLE (0x2).
   *    Type: struct movable_operations *.
   *    Reason: "Movable" pages (e.g. zram/balloon).
   *
   * 4. If bits == 3 (11) -> PAGE_MAPPING_KSM (0x3).
   *    Type: struct ksm_stable_node *.
   *    Math: 0x1 | 0x2 = 0x3. "Combination of prior two".
   *
   * INSTRUCTION: Use printk to report the state and the clean pointer.
   * WARNING: Do NOT dereference KSM/Movable pointers (incomplete types).
   */

  // WRITE YOUR CODE HERE:
  /*
   * TODO 5: DETECT COMPOUND TAIL PAGE
   * ---------------------------------
   * AXIOM: Tail pages have Bit 0 set in `compound_head`.
   * AXIOM: `compound_head` overlaps `lru.next`.
   * WORK: Read the first word of struct page.
   */
  unsigned long head_val = page->compound_head; // Access via Union

  if (head_val & 1) {
    printk(KERN_INFO "MAPPING_DECODE: [TAIL PAGE DETECTED]\n");
    printk(KERN_INFO "MAPPING_DECODE: compound_head = 0x%lx (Bit 0 set)\n",
           head_val);

    struct page *head_page = (struct page *)(head_val - 1);
    printk(KERN_INFO "MAPPING_DECODE: Head Page Ptr = 0x%px\n", head_page);

    // CHECK TAIL_MAPPING
    if (raw_mapping == (unsigned long)TAIL_MAPPING) {
      printk(KERN_INFO
             "MAPPING_DECODE: Mapping == TAIL_MAPPING (0x400) [Confirmed]\n");
    } else {
      printk(KERN_ERR "MAPPING_DECODE: Mapping (0x%lx) != TAIL_MAPPING (0x%lx) "
                      "[Unexpected!]\n",
             raw_mapping, (unsigned long)TAIL_MAPPING);
    }
    return; // Stop here for tail pages
  }

  // Normal Page Logic ...
  if (flags == 0) {
    printk(KERN_INFO "MAPPING_DECODE: Page is aligned (8-byte).\n");
    printk(KERN_INFO "MAPPING_DECODE: Clean Pointer = 0x%lx\n", clean_ptr);
  } else if (flags == 1) {
    printk(KERN_INFO "MAPPING_DECODE: Page is anonymous.\n");
    printk(KERN_INFO "MAPPING_DECODE: Clean Pointer = 0x%lx\n", clean_ptr);
  } else if (flags == 2) {
    printk(KERN_INFO "MAPPING_DECODE: Page is movable.\n");
    printk(KERN_INFO "MAPPING_DECODE: Clean Pointer = 0x%lx\n", clean_ptr);
  } else if (flags == 3) {
    printk(KERN_INFO "MAPPING_DECODE: Page is ksm.\n");
    printk(KERN_INFO "MAPPING_DECODE: Clean Pointer = 0x%lx\n", clean_ptr);
  }
}

static int __init mapping_init(void) {
  struct page *page;

  if (target_pid == 0 || target_va == 0) {
    printk(KERN_ERR "MAPPING_DECODE: Usage: insmod mapping_hw.ko target_pid=X "
                    "target_va=Y\n");
    return -EINVAL;
  }

  printk(KERN_INFO "MAPPING_DECODE: Init. PID=%d VA=0x%lx\n", target_pid,
         target_va);

  page = get_user_page(target_pid, target_va);
  if (!page) {
    printk(KERN_ERR "MAPPING_DECODE: Failed to get page.\n");
    return -EINVAL;
  }

  inspect_mapping(page);
  put_page(page);

  return 0;
}

static void __exit mapping_exit(void) {
  printk(KERN_INFO "MAPPING_DECODE: Exit.\n");
}

module_init(mapping_init);
module_exit(mapping_exit);
