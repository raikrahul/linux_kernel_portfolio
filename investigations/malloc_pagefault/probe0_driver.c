
#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

/*
 * PROBE 0: lock_vma_under_rcu
 * AXIOM: First reachable function receiving the fault address.
 * ABI: Arg 2 (address) -> RSI
 */

static int pid_filter = 0;
module_param(pid_filter, int, 0644);

static unsigned long addr_filter = 0;
module_param(addr_filter, ulong, 0644);

static struct kprobe kp = {
    .symbol_name = "lock_vma_under_rcu",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs) {
  unsigned long address = regs->si;

  if (pid_filter != 0 && current->pid != pid_filter)
    return 0;

  if (addr_filter != 0 && address != addr_filter)
    return 0;

  pr_info("PROBE_0_HIT: PID=%d ADDR=0x%lx\n", current->pid, address);
  return 0;
}

static int __init probe0_init(void) {
  kp.pre_handler = handler_pre;
  int ret = register_kprobe(&kp);
  if (ret < 0) {
    pr_err("PROBE_0_FAIL: register_kprobe failed, returned %d\n", ret);
    return ret;
  }
  pr_info("PROBE_0_LOADED: lock_vma_under_rcu\n");
  return 0;
}

static void __exit probe0_exit(void) {
  unregister_kprobe(&kp);
  pr_info("PROBE_0_UNLOADED\n");
}

module_init(probe0_init);
module_exit(probe0_exit);
MODULE_LICENSE("GPL");
