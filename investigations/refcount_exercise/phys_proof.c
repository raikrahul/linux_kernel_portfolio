/*01.AXIOM:VirtAddr=48bits_on_x86_64→split_into_indices_for_translation*/
/*02.AXIOM:CR3_register→points_to_Physical_PGD_Base*/
/*03.AXIOM:PGD_Index=bits[47:39]→9bits→512_entries*/
/*04.AXIOM:PUD_Index=bits[38:30]→9bits→512_entries*/
/*05.AXIOM:PMD_Index=bits[29:21]→9bits→512_entries*/
/*06.AXIOM:PTE_Index=bits[20:12]→9bits→512_entries*/
/*07.AXIOM:Page_Offset=bits[11:0]→12bits→4096_bytes*/
/*08.EXAMPLE:VirtAddr=0x7e11e3431000_(from_your_trace)*/
/*09.CALCULATE_BY_HAND:0x7e11e3431000_in_binary=0111_1110_0001_0001_1110_0011_0100_0011_0001_0000_0000_0000*/
/*10.SPLIT:*/
/*   PGD_IDX = 0111_1110_0 (binary) = 0xFC = 252*/
/*   PUD_IDX = 001_0001_11 (binary) = 0x87 = 135*/
/*   PMD_IDX = 10_0011_010 (binary) = 0x11A = 282*/
/*   PTE_IDX = 0_0011_0001 (binary) = 0x31 = 49*/
/*   OFFSET  = 0000_0000_0000 (binary) = 0*/

#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/pgtable.h>
#include <linux/pid.h>
#include <linux/sched.h>

static int target_pid = 0;
module_param(target_pid, int, 0644); /*TODO:PASS_PID_HERE*/
static unsigned long target_addr = 0;
module_param(target_addr, ulong, 0644); /*TODO:PASS_VIRT_ADDR_HERE*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Manual Page Table Walk Proof");

static int __init proof_init(void) {
  struct pid *pid_struct;
  struct task_struct *task;
  struct mm_struct *mm;
  pgd_t *pgd;
  p4d_t *p4d;
  pud_t *pud;
  pmd_t *pmd;
  pte_t *pte;
  unsigned long phys;

  printk(KERN_INFO "PROOF:init: pid=%d addr=%lx\n", target_pid, target_addr);

  /*TODO01:GET_PID_STRUCT:find_get_pid(pid_num)→returns_struct_pid_ptr*/
  pid_struct = find_get_pid(target_pid);
  if (!pid_struct)
    return -ESRCH;

  /*TODO02:GET_TASK_STRUCT:get_pid_task(pid_struct,
   * PIDTYPE_PID)→returns_task_struct_ptr*/
  task = get_pid_task(pid_struct, PIDTYPE_PID);
  if (!task)
    return -ESRCH;

  /*RRRO03:GET_MM_STRUCT:task->mm→describes_process_memory_space*/
  mm = task->mm;
  if (!mm) {
    put_task_struct(task);
    return -EINVAL;
  }

  /*LOCK:mmap_read_lock(mm)→prevent_changes_while_walking*/
  mmap_read_lock(mm);

  /*TODO04:DERIVE_PGD:pgd_offset(mm, addr)→uses_mm->pgd_and_addr_bits[47:39]*/
  pgd = pgd_offset(mm, target_addr);
  if (pgd_none(*pgd) || pgd_bad(*pgd)) {
    printk("PROOF:BAD_PGD\n");
    goto out;
  }

  /*TODO05:DERIVE_P4D:p4d_offset(pgd,
   * addr)→on_4level_paging_P4D=PGD→just_wraps_it*/
  p4d = p4d_offset(pgd, target_addr);
  if (p4d_none(*p4d) || p4d_bad(*p4d)) {
    printk("PROOF:BAD_P4D\n");
    goto out;
  }

  /*TODO06:DERIVE_PUD:pud_offset(p4d, addr)→uses_p4d_and_addr_bits[38:30]*/
  pud = pud_offset(p4d, target_addr);
  if (pud_none(*pud) || pud_bad(*pud)) {
    printk("PROOF:BAD_PUD\n");
    goto out;
  }

  /*TODO07:DERIVE_PMD:pmd_offset(pud, addr)→uses_pud_and_addr_bits[29:21]*/
  pmd = pmd_offset(pud, target_addr);
  if (pmd_none(*pmd) || pmd_bad(*pmd)) {
    printk("PROOF:BAD_PMD\n");
    goto out;
  }

  /*TODO08:DERIVE_PTE:pte_offset_kernel(pmd,
   * addr)→WARNING:pte_offset_map_might_fail_linking→TRY_pte_offset_kernel*/
  /*Why_pte_offset_kernel?→returns_direct_mapped_address_without_kmap_overhead*/
  pte = pte_offset_kernel(pmd, target_addr);
  if (!pte) {
    printk("PROOF:BAD_PTE\n");
    goto out;
  }

  /*TODO09:EXTRACT_PHYSICAL:pte_pfn(*pte) << PAGE_SHIFT → PFN * 4096 =
   * PhysAddr*/
  phys = pte_pfn(*pte) << PAGE_SHIFT;
  printk(KERN_INFO "PROOF:RESULT: pid=%d virt=%lx -> phys=%lx\n", target_pid,
         target_addr, phys);

out:
  mmap_read_unlock(mm);
  put_task_struct(task);
  put_pid(pid_struct);
  return 0;
}

static void __exit proof_exit(void) { printk(KERN_INFO "PROOF:exit\n"); }
module_init(proof_init);
module_exit(proof_exit);
