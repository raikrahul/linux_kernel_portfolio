/*
 * lru_mlock_hw.c - INSPECT LRU vs MLOCK_COUNT UNION
 *
 * AXIOM 01: struct page bytes [8-23] = 16 bytes = union
 * AXIOM 02: lru interpretation: bytes [8-15] = next ptr, bytes [16-23] = prev
 * ptr AXIOM 03: mlock interpretation: bytes [8-15] = __filler, bytes [16-19] =
 * mlock_count AXIOM 04: mlock_count is unsigned int = 4 bytes, at offset 16
 * relative to lru union start
 *
 * KERNEL SOURCE:
 * /usr/src/linux-headers-6.14.0-37-generic/include/linux/mm_types.h Line 89:
 * struct list_head lru; Line 93:  void *__filler; Line 96:  unsigned int
 * mlock_count;
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("LRU vs mlock_count Union Inspector");

static int target_pid = 0;
static unsigned long target_addr = 0;

module_param(target_pid, int, 0);
module_param(target_addr, ulong, 0);

static struct page *get_page_from_mm(struct mm_struct *mm, unsigned long addr) {
  struct page *page = NULL;
  int ret;
  ret = get_user_pages_remote(mm, addr, 1, 0, &page, NULL);
  if (ret < 0)
    return NULL;
  return page;
}

static void inspect_lru_mlock(struct page *page) {
  unsigned long lru_next_raw, lru_prev_raw;
  unsigned int mlock_val;
  unsigned long flags_raw;

  if (!page)
    return;

  printk(KERN_INFO "LruMlock: Page Ptr = %px\n", page);

  /*
   * MEMORY LAYOUT OF BYTES [8-23] OF struct page:
   * ┌─────────────────────────────────────────────────────────────┐
   * │ OFFSET 8-15 (8 bytes): lru.next OR __filler                 │
   * │ OFFSET 16-23 (8 bytes): lru.prev                            │
   * │     BUT mlock_count is only at OFFSET 16-19 (4 bytes)       │
   * └─────────────────────────────────────────────────────────────┘
   */

  /*
   * TODO 1: Read lru.next (bytes [8-15] as pointer)
   * -------------------------------------------------------------------------
   * AXIOM: page->lru is struct list_head, lru.next is first field (offset 0)
   * AXIOM: struct list_head { next, prev } = 8 + 8 = 16 bytes
   * CALCULATION: page + 8 bytes → lru.next, page + 16 bytes → lru.prev
   *
   * WRITE THE CODE:
   * lru_next_raw = (unsigned long)page->lru.next;
   * -------------------------------------------------------------------------
   */

  lru_next_raw = (unsigned long)page->lru.next;

  /*
   * TODO 2: Read lru.prev (bytes [16-23] as pointer)
   * -------------------------------------------------------------------------
   * AXIOM: lru.prev is second field of struct list_head
   *
   * WRITE THE CODE:
   * lru_prev_raw = (unsigned long)page->lru.prev;
   * -------------------------------------------------------------------------
   */

  lru_prev_raw = (unsigned long)page->lru.prev;

  /*
   * TODO 3: Read mlock_count (reinterpret bytes [16-19] as integer)
   * -------------------------------------------------------------------------
   * PROBLEM: mlock_count shares bytes [16-19] with lower 4 bytes of lru.prev
   * KERNEL SOURCE line 93-96: struct { void *__filler; unsigned int
   * mlock_count; } CALCULATION: __filler = 8 bytes (offset 8-15), mlock_count =
   * 4 bytes (offset 16-19)
   *
   * HOW TO ACCESS: cannot directly access mlock_count, must cast or use raw
   * pointer HACK: unsigned int *mlock_ptr = (unsigned int *)((char *)page + 8 +
   * 8); → page base + 8 (skip flags) + 8 (skip __filler) = offset to
   * mlock_count
   *
   * DANGER: if page is on LRU, mlock_count reads garbage (lru.prev bits as int)
   *
   * WRITE THE CODE (raw byte access):
   * mlock_val = *((unsigned int *)((char *)&page->lru + 8));
   * -------------------------------------------------------------------------
   */

  mlock_val = *((unsigned int *)((char *)&page->lru + 8));

  /*
   * TODO 4: Read page flags to check page state
   * -------------------------------------------------------------------------
   * AXIOM: page->flags at offset 0, tells if page is mlocked
   *
   * WRITE THE CODE:
   * flags_raw = page->flags;
   * -------------------------------------------------------------------------
   */

  flags_raw = page->flags;

  printk(KERN_INFO "LruMlock: lru.next     = 0x%lx\n", lru_next_raw);
  printk(KERN_INFO "LruMlock: lru.prev     = 0x%lx\n", lru_prev_raw);
  printk(KERN_INFO
         "LruMlock: mlock_count  = %u (same bytes as lru.prev lower 32 bits)\n",
         mlock_val);
  printk(KERN_INFO "LruMlock: flags        = 0x%lx\n", flags_raw);

  /*
   * INTERPRETATION:
   * IF page is on LRU list: lru.next, lru.prev = valid 0xffff... pointers
   *                         mlock_count = garbage (pointer bits as integer)
   * IF page is mlocked:     lru.next, lru.prev = may be poison or stale
   *                         mlock_count = 1, 2, 3... (valid count)
   */

  put_page(page);
}

static int __init lru_mlock_init(void) {
  struct pid *pid_struct;
  struct task_struct *task;
  struct page *page;

  printk(KERN_INFO "LruMlock: Init. Target PID=%d, Addr=0x%lx\n", target_pid,
         target_addr);

  if (target_pid == 0 || target_addr == 0)
    return -EINVAL;

  pid_struct = find_get_pid(target_pid);
  if (!pid_struct)
    return -ESRCH;
  task = pid_task(pid_struct, PIDTYPE_PID);
  if (!task)
    return -ESRCH;

  page = get_page_from_mm(task->mm, target_addr);
  inspect_lru_mlock(page);

  return 0;
}

static void __exit lru_mlock_exit(void) {
  printk(KERN_INFO "LruMlock: Exit.\n");
}

module_init(lru_mlock_init);
module_exit(lru_mlock_exit);
