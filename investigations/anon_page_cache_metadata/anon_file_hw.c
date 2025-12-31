/*
 * anon_file_hw.c - INSPECTING LISTING 2-3 (ANON/FILE METADATA)
 *
 * AXIOM 0: struct page (64 bytes) is ONE physical frame of RAM.
 * AXIOM 1: `page->mapping` (offset 24) is a pointer to `struct address_space`.
 * AXIOM 2: Pointers to structs are ALIGNED (bottom 2 bits are 00).
 * AXIOM 3: Kernel reuses bottom 2 bits for FLAGS:
 *    - Bit 0 = 1 -> PAGE_MAPPING_ANON (Anonymous).
 *    - Bit 1 = 1 -> PAGE_MAPPING_MOVABLE (KSM/Movable).
 *
 * YOUR MISSION:
 * 1. Read the raw `mapping` value.
 * 2. Manually extract the flags (Bit 0).
 * 3. Manually MASK the flags to get the clean pointer.
 * 4. Verify axioms: Anon pages have bit 0 set, File pages do not.
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Anon vs File Metadata Inspector");

static int target_pid = 0;
static unsigned long anon_addr = 0;
static unsigned long file_addr = 0;

module_param(target_pid, int, 0);
module_param(anon_addr, ulong, 0);
module_param(file_addr, ulong, 0);

static struct page *get_page_from_mm(struct mm_struct *mm, unsigned long addr) {
  struct page *page = NULL;
  int ret;
  ret = get_user_pages_remote(mm, addr, 1, 0, &page, NULL);
  if (ret < 0)
    return NULL;
  return page;
}

static void inspect_page(struct page *page, const char *label) {
  unsigned long raw_mapping;
  unsigned long bit_0;
  unsigned long clean_pointer;
  unsigned long lru_prev, lru_next;

  if (!page)
    return;

  printk(KERN_INFO "MetaHW: === INSPECTING %s PAGE ===\n", label);
  printk(KERN_INFO "MetaHW: Page Ptr = %px\n", page);

  /* AXIOM: page->mapping is at offset 24 (on standard x86_64, usually).
   * We access it via the struct field directly. */

  /*
   * TODO 1: Read Raw Mapping
   * -------------------------------------------------------------------------
   * CAST `page->mapping` to `unsigned long` so we can see the bits.
   * If you look at it as a pointer, %p might mask bits for you!
   * We want RAW HEX.
   *
   * CODE:
   * raw_mapping = (unsigned long)page->mapping;
   * -------------------------------------------------------------------------
   */

  // raw_mapping = ... /* WRITE THIS LINE */
  /* Fallback so printk doesn't crash if empty */
  raw_mapping = (unsigned long)page->mapping;
  if (raw_mapping == 0)
    raw_mapping = (unsigned long)page->mapping;

  /*
   * TODO 2: Extract Bit 0
   * -------------------------------------------------------------------------
   * AXIOM: Bit 0 is the Least Significant Bit.
   * OPERATION: Bitwise AND with 1.
   *
   * CODE:
   * bit_0 = raw_mapping & 1;
   * -------------------------------------------------------------------------
   */

  // bit_0 = ... /* WRITE THIS LINE */
  bit_0 = raw_mapping & 1;

  /*
   * TODO 3: Create Clean Pointer
   * -------------------------------------------------------------------------
   * AXIOM: To get the real address, we must zero out the bottom 2 bits.
   * OPERATION: Bitwise AND with MASK.
   * MASK: ~3 (Binary ...111100).
   *
   * CODE:
   * clean_pointer = raw_mapping & ~3UL;
   * -------------------------------------------------------------------------
   */

  // clean_pointer = ... /* WRITE THIS LINE */
  clean_pointer = raw_mapping & ~3UL;

  printk(KERN_INFO "MetaHW: Raw Mapping   = 0x%lx\n", raw_mapping);
  printk(KERN_INFO "MetaHW: Bit 0 (Anon?) = %lu\n", bit_0);
  printk(KERN_INFO "MetaHW: Clean Pointer = 0x%lx\n", clean_pointer);

  /*
   * TODO 4: Inspect Index
   * -------------------------------------------------------------------------
   * AXIOM: page->index is simple integer (pgoff_t).
   * CODE:
   * raw_index = page->index;
   * -------------------------------------------------------------------------
   */
  printk(KERN_INFO "MetaHW: Index         = %lu\n", page->index);

  /*
   * CHECKING LRU POINTERS
   * To see if they look like Valid Kernel Addresses (0xffff...)
   */
  lru_prev = (unsigned long)page->lru.prev;
  lru_next = (unsigned long)page->lru.next;
  printk(KERN_INFO "MetaHW: LRU.prev      = 0x%lx\n", lru_prev);
  printk(KERN_INFO "MetaHW: LRU.next      = 0x%lx\n", lru_next);

  put_page(page);
}

static int __init metadata_hw_init(void) {
  struct pid *pid_struct;
  struct task_struct *task;
  struct page *p_anon, *p_file;

  printk(KERN_INFO "MetaHW: Init. Target PID=%d\n", target_pid);

  if (target_pid == 0)
    return -EINVAL;

  pid_struct = find_get_pid(target_pid);
  if (!pid_struct)
    return -ESRCH;
  task = pid_task(pid_struct, PIDTYPE_PID);
  if (!task)
    return -ESRCH;

  if (anon_addr) {
    p_anon = get_page_from_mm(task->mm, anon_addr);
    inspect_page(p_anon, "ANONYMOUS");
  }

  if (file_addr) {
    p_file = get_page_from_mm(task->mm, file_addr);
    inspect_page(p_file, "FILE-BACKED");
  }

  return 0;
}

static void __exit metadata_hw_exit(void) {
  printk(KERN_INFO "MetaHW: Exit.\n");
}

module_init(metadata_hw_init);
module_exit(metadata_hw_exit);
