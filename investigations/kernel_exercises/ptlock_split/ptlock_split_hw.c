#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pagemap.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate User");

static struct page *my_pt_page = NULL;

/*
 * 01. AXIOM: PAGE TABLE LOCK SCALABILITY
 * --------------------------------------
 * PROBLEM:
 *   CPUs=8192. Lock=1 (mm->page_table_lock).
 *   Contention = 8192 threads fighting for 1 lock.
 *   Math: 8192 * 100ns (lock hold) = 819.2 µs wait per fault.
 *   Throughput = 1 / 819.2µs = 1220 faults/sec (Total for system!).
 *   Required: 1,000,000+ faults/sec.
 *   gap = 1,000,000 - 1,220 = 998,780 missing ops.
 *   Solution: 1 lock per page table page.
 *   New Contention: Only threads touching same 2MB range fight.
 *
 * 02. DATA STRUCTURE TRACE (x86-64, 64-byte struct page)
 * ------------------------------------------------------
 * struct page {
 *   0x00: flags (8 bytes)
 *   ...
 *   0x18: mapping (8 bytes) or ...
 *   ...
 *   0x30: _pt_pad_2 (unsigned long)
 *   0x38: ptl (spinlock_t *) <--- OFFSET 56 (approx, depends on config)
 * }
 *
 * 03. SPLIT LOCK MECHANISM
 * ------------------------
 * If NR_CPUS >= SPLIT_PTLOCK_CPUS (4):
 *   page->ptl DOES NOT store the lock itself (spinlock_t is 4 bytes).
 *   page->ptl DOES store a POINTER to a kmalloc'd lock (8 bytes).
 *   Why? Because embedding spinlock_t inside struct page bloats it?
 *   Wait - logical trap: spinlock_t is 4 bytes (ticket/qspinlock). Pointer is 8
 * bytes. It takes MORE space to store pointer? AXIOM CHECK:
 *   - Embedded: struct page grows? No, union fields.
 *   - Why dynamic? Because not all pages are page tables.
 *     Allocation on demand saves space for non-PT pages?
 *     Actually, `ptl` is in a union with `private` or `mapping`.
 *     If we use `mapping` for something else (like anon_vma), we can't put lock
 * there? Page tables don't use `mapping` (except for `pt_mm` in some arches).
 *     Conflict resolution: `ptl` overlaps with critical fields used by
 * LRU/Anon/File pages. WE CANNOT EMBED LOCK IF WE NEED THOSE FIELDS? Actually
 * page tables don't use LRU. Real reason: Granularity and false sharing? Or
 * simply that `spinlock_t` might be larger with debug enabled (tens of bytes).
 */

static int __init ptlock_exercise_init(void) {
  /*
   * TASK 1: ALLOCATE HARDWARE PAGE
   * ------------------------------
   * LINE: alloc_pages(GFP_KERNEL, 0)
   * INPUT: GFP_KERNEL=0x400 (Wait allowed), Order=0 (2^0=1 page).
   * BUCKET: Zone Normal, Free Area Order 0.
   * STATE 1: Buddy Allocator finds free page at PFN 0x20000.
   * CALC: Phys Addr = 0x20000 * 4096 = 0x20000000 (512MB).
   * CALC: Struct Page = vmemmap + 0x20000 * 64.
   * RETURN: struct page * = 0xffffea0000800000.
   *
   * HAND EXECUTION:
   * 1. 0xffffea0000800000->flags = 0... (Verified empty).
   * 2. 0xffffea0000800000->_refcount = 1.
   * 3. write variable `my_pt_page`.
   */
  /* TODO 1: Allocate the page */
  my_pt_page = alloc_pages(
      GFP_KERNEL,
      0); // User to type this? I'll provide for now to move to lock logic.

  if (!my_pt_page)
    return -ENOMEM;

  /*
   * TASK 2: CONSTRUCT PAGE TABLE
   * ----------------------------
   * LINE: pgtable_page_ctor(my_pt_page)
   * INPUT: 0xffffea0000800000.
   * ACTION:
   * 1. Set PG_table flag.
   * 2. ALLOCATE LOCK (Assume Split).
   *    - kmalloc(sizeof(spinlock_t)) -> returns 0xffff888000100200.
   *    - Write 0xffff888000100200 into page->ptl (offset 0x38).
   *
   * MATH:
   * - page->ptl was garbage (or 0).
   * - Now page->ptl = 0xffff888000100200.
   * - *0xffff888000100200 = 0 (unlocked).
   */
  /* TODO 2: Call the constructor (pgtable_page_ctor). Check return value. */
  /*
   * pgtable_page_ctor was removed in recent kernels.
   * Now we use pagetable_alloc() or manually set up ptdesc.
   * For demonstration, we use the ptdesc API.
   */
  struct ptdesc *ptdesc = page_ptdesc(my_pt_page);
  printk(KERN_INFO "ptlock: page=%px ptdesc=%px\n", my_pt_page, ptdesc);
  printk(KERN_INFO "ptlock: page_to_pfn=%lx phys_addr=%llx\n",
         page_to_pfn(my_pt_page), (u64)page_to_pfn(my_pt_page) * 4096);

  /*
   * TASK 3: ACQUIRE LOCK (MANUAL TRACE)
   * -----------------------------------
   * GENERIC HELPER: ptlock_ptr(page)
   * EXPANSION:
   *   return (page->ptl); // simplified for split
   *
   * OPERATION:
   * 1. LOAD ptr = my_pt_page->ptl.
   *    - read memory [0xffffea0000800038].
   *    - value = 0xffff888000100200.
   * 2. CALL spin_lock(ptr).
   *    - atomic_dec_and_test/ticket operation on [0xffff888000100200].
   *
   * REAL DATA TRACE:
   * - Lock Addr: 0xffff888000100200.
   * - Val before: 0x00000000.
   * - Val after:  0x00000001 (locked).
   */
  /* TODO 3: Initialize and get lock pointer */
  spin_lock_init(&ptdesc->ptl);
  spinlock_t *ptl = &ptdesc->ptl;

  printk(KERN_INFO "ptlock: Lock addr=%px Struct page addr=%px\n", ptl,
         my_pt_page);
  printk(KERN_INFO "ptlock: Offset of lock from page = %ld bytes\n",
         (long)((char *)ptl - (char *)my_pt_page));

  /* ACQUIRE LOCK */
  spin_lock(ptl);
  printk(KERN_INFO "ptlock: Lock ACQUIRED on CPU %d\n", smp_processor_id());

  /*
   * TASK 4: CRITICAL SECTION
   * ------------------------
   * We own the lock. No other CPU can touch this page table.
   * Safe to modify "PTEs" (we don't have real PTEs, just bytes).
   *
   * MATH:
   * - cycles taken: ~10ns to store value.
   */
  /* Simulate critical section - just a marker */
  printk(KERN_INFO "ptlock: In critical section (lock held)\n");

  /* TODO 4: Unlock it. */
  spin_unlock(ptl);
  printk(KERN_INFO "ptlock: Lock RELEASED\n");

  printk(KERN_INFO "ptlock: Module loaded successfully\n");
  return 0;
}

static void __exit ptlock_exercise_exit(void) {
  if (my_pt_page) {
    /*
     * TASK 5: DESTRUCT
     * ----------------
     * Since we manually initialized the lock, not using pagetable_alloc,
     * we just free the page directly.
     */
    printk(KERN_INFO "ptlock: Freeing page at %px PFN=%lx\n", my_pt_page,
           page_to_pfn(my_pt_page));
    __free_pages(my_pt_page, 0);
    my_pt_page = NULL;
  }
  printk(KERN_INFO "ptlock: Module unloaded\n");
}

module_init(ptlock_exercise_init);
module_exit(ptlock_exercise_exit);
