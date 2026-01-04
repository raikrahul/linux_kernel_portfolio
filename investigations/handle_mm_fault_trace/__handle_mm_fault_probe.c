#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Antigravity");
MODULE_DESCRIPTION("Probe __handle_mm_fault for axiomatic trace");

static int target_pid = 0;
module_param(target_pid, int, 0644);

static unsigned long target_addr = 0;
module_param(target_addr, ulong, 0644);

/*
 * PROBLEM: How to verify the kernel reaches the core page fault logic?
 * SOLUTION: Plant a kprobe on __handle_mm_fault.
 * INPUT: RDI=vma, RSI=address, RDX=flags.
 */

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
  /* AXIOM: current->pid is the ID of the thread currently on the CPU core. */
  int pid = current->pid;
  /* AXIOM: RSI on x86_64 holds the 2nd argument of the function call. */
  unsigned long address = regs->si;
  /* AXIOM: RDX on x86_64 holds the 3rd argument of the function call. */
  unsigned int flags = (unsigned int)regs->dx;

  /* TODO 1: Implement the PID filter.
   * HINT: Compare pid to target_pid. If mismatch, return 0.
   */

  /* TODO 2: Implement the Address mask check.
   * HINT: Use (address & PAGE_MASK) == (target_addr & PAGE_MASK).
   */

  pr_info("AXIOM_TRACE: Hit __handle_mm_fault!\n");
  pr_info("   COMM: %s | PID: %d\n", current->comm, pid);
  pr_info("   ADDR: 0x%lx | FLAGS: 0x%x\n", address, flags);

  /*
   * DERIVATION OF FLAGS (Exercise for USER):
   * 1. 0x41 -> FAULT_FLAG_USER (0x40) + FAULT_FLAG_WRITE (0x01).
   * 2. 0x40 -> FAULT_FLAG_USER (0x40) - implies Read Fault.
   */

  return 0;
}

static struct kprobe kp = {
    .symbol_name = "__handle_mm_fault",
    .pre_handler = handler_pre,
};

static int __init probe_init(void) {
  int ret;
  ret = register_kprobe(&kp);
  if (ret < 0) {
    pr_err("register_kprobe failed, returned %d\n", ret);
    return ret;
  }
  pr_info("Probe planted: __handle_mm_fault at 0x%p\n", kp.addr);
  return 0;
}

static void __exit probe_exit(void) {
  unregister_kprobe(&kp);
  pr_info("Probe removed: __handle_mm_fault\n");
}

module_init(probe_init);
module_exit(probe_exit);
