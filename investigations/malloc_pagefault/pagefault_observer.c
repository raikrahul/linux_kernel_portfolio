/*
 * pagefault_observer.c - Kprobe-based page fault observer
 *
 * ================================================================================
 * WHAT: Kernel module that intercepts handle_mm_fault() calls using kprobe.
 *       handle_mm_fault = function at memory.c:6470.
 *       Kprobe = insert breakpoint at function address 0xffffffff812xxxxx.
 *       When CPU executes that address → control transfers to our handler.
 *
 * WHY: To observe page faults BEFORE and AFTER they happen.
 *      Without this: Cannot see kernel internals from userspace.
 *      With this: See PID, address, VMA, flags for every fault.
 *      Example: Process writes to 0x7f1234567000 → #PF → we see it here.
 *
 * WHERE: Runs in kernel space (ring 0).
 *        Probe point: handle_mm_fault at 0xffffffff812xxxxx.
 *        Output: /var/log/kern.log or dmesg.
 *
 * WHO: Any process that triggers page fault.
 *      Filter: if target_pid=1234, only PID 1234 faults are logged.
 *      current->pid = PID of faulting process.
 *
 * WHEN: Fires on EVERY call to handle_mm_fault.
 *       Frequency: 1000s per second on busy system.
 *       Timing: pre_handler runs in ~100-500 nanoseconds.
 *
 * WITHOUT: Without kprobe, must modify kernel source and recompile.
 *          Without pre_handler, cannot see state BEFORE fault.
 *          Without post_handler, cannot see return value.
 *
 * WHICH: handle_mm_fault (not do_page_fault, not __handle_mm_fault).
 *        handle_mm_fault = exported symbol (EXPORT_SYMBOL_GPL).
 *        __handle_mm_fault = static, cannot probe by name.
 * ================================================================================
 */

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("User Learning Kernel MM");
MODULE_DESCRIPTION("Kprobe observer for handle_mm_fault");

/*
 * target_pid: Filter to observe only this PID.
 * WHAT: Integer, set via insmod parameter.
 * WHY: System has 100+ processes faulting. Filter to 1 reduces noise.
 * EXAMPLE: insmod pagefault_observer.ko target_pid=5678
 *          → Only faults from PID 5678 are logged.
 * WITHOUT: target_pid=0 → ALL faults logged → 10000 lines/second.
 */
static int target_pid = 0;
module_param(target_pid, int, 0644);
MODULE_PARM_DESC(target_pid, "PID to observe page faults for");

/*
 * kp_fault: The kprobe structure.
 * WHAT: 96-byte struct containing:
 *       - symbol_name: "handle_mm_fault" (string pointer, 8 bytes)
 *       - addr: resolved address like 0xffffffff81234560 (8 bytes)
 *       - pre_handler: function pointer (8 bytes)
 *       - post_handler: function pointer (8 bytes)
 */
static struct kprobe kp_fault = {
    .symbol_name = "handle_mm_fault",
};

/*
 * kp_vma_link: Kprobe to observe VMA insertion into Maple Tree.
 * WHAT: Intercepts vma_link() calls during mmap().
 * WHY: Proves VMA is created during mmap(), NOT during page fault.
 * WHERE: mmap.c:395 - vma_link(struct mm_struct *mm, struct vm_area_struct
 * *vma) ARGS: RDI = mm, RSI = vma.
 */
static struct kprobe kp_vma_link = {
    .symbol_name = "vma_link",
};

/*
 * kp_mmap_region: Kprobe to observe ALL mmap() calls including anonymous.
 * WHAT: Intercepts __mmap_region() which creates VMA for ALL mmap types.
 * WHY: vma_link() not called for all paths. __mmap_region is the common path.
 * WHERE: mmap.c:2727 - __mmap_region(file, addr, len, vm_flags, pgoff, uf)
 * ARGS: RDI = file (NULL for anonymous), RSI = addr, RDX = len.
 */
static struct kprobe kp_mmap_region = {
    .symbol_name = "__mmap_region",
};

/*
 * handler_pre: Called BEFORE handle_mm_fault executes.
 *
 * WHAT: Extract arguments from pt_regs and log them.
 *
 * WHY: See fault parameters BEFORE kernel processes them.
 *
 * WHERE: pt_regs layout on x86_64:
 *        Offset 0x70 (112): regs->di = RDI = arg1 = vma pointer
 *        Offset 0x68 (104): regs->si = RSI = arg2 = faulting address
 *        Offset 0x60 (96):  regs->dx = RDX = arg3 = flags
 *
 * WHO: current = task_struct of faulting process.
 *      current->pid = 1234 (example).
 *      current->comm = "mm_exercise_user" (16-char name).
 *
 * WHEN: Runs with interrupts disabled, preemption disabled.
 *       Duration: 100-500 nanoseconds for this handler.
 *
 * EXAMPLE:
 *   Process PID=1234 writes to VA=0x7f0012345678.
 *   CPU raises #PF (exception 14).
 *   do_page_fault() calls handle_mm_fault(vma, 0x7f0012345678, 0x55, regs).
 *   BEFORE handle_mm_fault runs, our handler_pre fires:
 *     regs->di = 0xffff888012340000 (vma pointer)
 *     regs->si = 0x7f0012345678 (faulting address)
 *     regs->dx = 0x55 (flags: FAULT_FLAG_WRITE | FAULT_FLAG_USER | ...)
 *     current->pid = 1234
 *     vma->vm_start = 0x7f0012340000
 *     vma->vm_end = 0x7f0012350000
 *   We log: "[PF_OBS] PID=1234 ADDR=0x7f0012345678
 * VMA=[0x7f0012340000-0x7f0012350000]"
 *
 * WITHOUT: Cannot see arguments. Cannot filter by PID. Cannot correlate with
 * VMA.
 *
 * RETURN VALUE:
 *   0 = continue to handle_mm_fault.
 *   Non-zero = skip handle_mm_fault (dangerous, never do this).
 */
static int __kprobes handler_pre(struct kprobe *p, struct pt_regs *regs) {
  /*
   * vma: Virtual Memory Area descriptor.
   * WHAT: Pointer to struct vm_area_struct.
   * SIZE: 8 bytes (pointer on x86_64).
   * VALUE: e.g., 0xffff888012340000.
   * CONTAINS: vm_start, vm_end, vm_flags, vm_mm.
   */
  struct vm_area_struct *vma;

  /*
   * address: Faulting virtual address.
   * WHAT: 64-bit unsigned integer.
   * VALUE: e.g., 0x7f0012345678.
   * RANGE: 0x0000000000000000 to 0x00007fffffffffff (userspace).
   *        0xffff800000000000 to 0xffffffffffffffff (kernelspace).
   */
  unsigned long address;

  /*
   * flags: Fault flags.
   * WHAT: 32-bit bitmask.
   * BIT 0 (0x01): FAULT_FLAG_WRITE = write fault.
   * BIT 1 (0x02): FAULT_FLAG_MKWRITE = COW write.
   * BIT 4 (0x10): FAULT_FLAG_USER = userspace access.
   * BIT 8 (0x100): FAULT_FLAG_REMOTE = remote access.
   * EXAMPLE: flags=0x55 = 0101_0101 = WRITE | USER | ...
   */
  unsigned int flags;

  /*
   * Filter by PID.
   * WHAT: Compare current->pid with target_pid.
   * WHY: Reduce log noise from 10000/sec to 1-10/sec.
   * EXAMPLE: target_pid=1234, current->pid=5678 → skip.
   *          target_pid=1234, current->pid=1234 → log.
   *          target_pid=0 → log all (no filter).
   */
  if (target_pid != 0 && current->pid != target_pid)
    return 0;

  /*
   * Extract arguments from registers.
   * x86_64 calling convention:
   *   Arg1 (vma)     → RDI → regs->di
   *   Arg2 (address) → RSI → regs->si
   *   Arg3 (flags)   → RDX → regs->dx
   *   Arg4 (pt_regs) → RCX → regs->cx
   *
   * NUMERICAL EXAMPLE:
   *   regs @ 0xffffc90001234000
   *   regs->di = *(uint64_t*)(0xffffc90001234000 + 112) = 0xffff888012340000
   *   regs->si = *(uint64_t*)(0xffffc90001234000 + 104) = 0x7f0012345678
   *   regs->dx = *(uint64_t*)(0xffffc90001234000 + 96)  = 0x0000000000000055
   */
  vma = (struct vm_area_struct *)regs->di;
  address = regs->si;
  flags = (unsigned int)regs->dx;

  /*
   * Log the fault.
   * FORMAT: [PF_OBS] PID=NNNN COMM=SSSS ADDR=0xXXXX FLAGS=0xXX
   * VMA=[0xXXXX-0xXXXX]
   *
   * EXAMPLE OUTPUT:
   * [PF_OBS] PID=1234 COMM=mm_exercise_use ADDR=0x7f0012345678 FLAGS=0x55
   * VMA=[0x7f0012340000-0x7f0012350000]
   *
   * FIELD SIZES:
   *   PID: 1-7 digits (1 to 4194304).
   *   COMM: 1-15 characters (task_struct->comm is char[16]).
   *   ADDR: 12-16 hex digits.
   *   FLAGS: 1-4 hex digits.
   *   VMA: 12-16 hex digits each.
   */
  /* Arg4 is user regs */
  struct pt_regs *user_regs = (struct pt_regs *)regs->cx;
  unsigned long user_ip = user_regs ? user_regs->ip : 0;
  unsigned long user_sp = user_regs ? user_regs->sp : 0;

  /* Stack Probing Logic (Derived Axiomatically)
   * 1. AXIOM: Every thread needs a private scratchpad (Stack).
   * 2. DEFINITION: 'current->stack' is the Base Address of this allocation.
   *    (Source: include/linux/sched.h, task_struct)
   * 3. DEFINITION: 'THREAD_SIZE' is the Size of this allocation.
   *    (Source: arch/x86/include/asm/page_64_types.h)
   * 4. CALCULATION: Top = Base + Size.
   * 5. MEASUREMENT: __builtin_frame_address(0) reads the CPU's RSP register.
   */
  void *kstack_base = current->stack;
  unsigned long kstack_top = (unsigned long)kstack_base + THREAD_SIZE;
  unsigned long current_rsp = (unsigned long)__builtin_frame_address(0);

  /*
   * Hardware Error Code (AXIOM DERIVATION)
   * Source: arch/x86/include/asm/ptrace.h, line 46:
   * "On interrupt, this [orig_ax] is the error code."
   */
  unsigned long hw_error = user_regs ? user_regs->orig_ax : 0;

  pr_info("[PF_OBS] PID=%d COMM=%s ADDR=0x%lx FLAGS=0x%x HW_ERROR=0x%lx "
          "VMA=[0x%lx-0x%lx] "
          "USER_RIP=0x%lx USER_RSP=0x%lx\n",
          current->pid, current->comm, address, flags, hw_error,
          vma ? vma->vm_start : 0, vma ? vma->vm_end : 0, user_ip, user_sp);

  pr_info("[PF_OBS] PROOF: Task=0x%px StackBase=0x%px StackTop=0x%lx "
          "CurrentKernelRSP=0x%lx THREAD_SIZE=%lu\n",
          current, kstack_base, kstack_top, current_rsp, THREAD_SIZE);

  return 0;
}

/*
 * vma_link_handler_pre: Called BEFORE vma_link executes.
 *
 * WHAT: Observe VMA insertion into Maple Tree during mmap().
 *
 * WHY: Prove that mmap() creates VMA entry, NOT page fault.
 *
 * WHERE: vma_link(struct mm_struct *mm, struct vm_area_struct *vma)
 *        x86_64: RDI = mm, RSI = vma.
 *
 * EXAMPLE:
 *   User calls mmap(NULL, 4096, PROT_READ|PROT_WRITE,
 * MAP_PRIVATE|MAP_ANONYMOUS, -1, 0). Kernel calls vma_link(mm, vma) where
 * vma->vm_start = 0x716b326c8000. Our probe fires and logs: [MAPLE_INSERT]
 * vm_start=0x716b326c8000 vm_end=0x716b326c9000.
 */
static int __kprobes vma_link_handler_pre(struct kprobe *p,
                                          struct pt_regs *regs) {
  struct vm_area_struct *vma;

  /* Filter by PID. */
  if (target_pid != 0 && current->pid != target_pid)
    return 0;

  /*
   * vma = 2nd argument = RSI register.
   * x86_64 calling convention: arg1=RDI, arg2=RSI, arg3=RDX, arg4=RCX.
   */
  vma = (struct vm_area_struct *)regs->si;

  if (vma) {
    pr_info("[MAPLE_INSERT] PID=%d COMM=%s vm_start=0x%lx vm_end=0x%lx "
            "vm_flags=0x%lx\n",
            current->pid, current->comm, vma->vm_start, vma->vm_end,
            vma->vm_flags);
  }

  return 0;
}

/*
 * mmap_region_handler_pre: Called BEFORE __mmap_region executes.
 *
 * WHAT: Observe ALL mmap() calls including anonymous.
 *
 * WHY: vma_link() not called for anonymous mmap path. __mmap_region is called
 *      for ALL mmap types and shows the address BEFORE VMA is created.
 *
 * WHERE: __mmap_region(file, addr, len, vm_flags, pgoff, uf)
 *        x86_64: RDI = file, RSI = addr, RDX = len, RCX = vm_flags.
 */
static int __kprobes mmap_region_handler_pre(struct kprobe *p,
                                             struct pt_regs *regs) {
  unsigned long file_ptr;
  unsigned long addr;
  unsigned long len;
  unsigned long vm_flags;

  /* Filter by PID. */
  if (target_pid != 0 && current->pid != target_pid)
    return 0;

  /*
   * __mmap_region(file, addr, len, vm_flags, pgoff, uf)
   * RDI = file (NULL for anonymous)
   * RSI = addr
   * RDX = len
   * RCX = vm_flags
   */
  file_ptr = regs->di;
  addr = regs->si;
  len = regs->dx;
  vm_flags = regs->cx;

  pr_info("[MMAP_REGION] PID=%d COMM=%s file=%s addr=0x%lx len=0x%lx "
          "vm_flags=0x%lx\n",
          current->pid, current->comm, file_ptr ? "FILE" : "ANON", addr, len,
          vm_flags);

  return 0;
}

/*
 * handler_post: Called AFTER handle_mm_fault returns.
 *
 * WHAT: Log the return value of handle_mm_fault.
 *
 * WHY: See if fault succeeded, failed, or triggered OOM.
 *
 * WHERE: regs->ax contains return value (x86_64 convention).
 *
 * RETURN VALUES (vm_fault_t = unsigned int):
 *   0x000 = success (no flags set).
 *   0x001 = VM_FAULT_OOM = out of memory.
 *   0x002 = VM_FAULT_SIGBUS = bus error.
 *   0x004 = VM_FAULT_MAJOR = major fault (disk I/O).
 *   0x008 = VM_FAULT_HWPOISON = hardware memory error.
 *   0x800 = VM_FAULT_COMPLETED = completed without page lock.
 *
 * EXAMPLE:
 *   ret=0x80000bb = 0000_1000_0000_0000_0000_0000_1011_1011
 *   Bit 0 (0x01): OOM? No.
 *   Bit 1 (0x02): SIGBUS? No.
 *   Bit 3 (0x08): HWPOISON? No.
 *   High bits: internal flags.
 */
static void __kprobes handler_post(struct kprobe *p, struct pt_regs *regs,
                                   unsigned long flags) {
  /*
   * Filter by PID.
   */
  if (target_pid != 0 && current->pid != target_pid)
    return;

  /*
   * Log return value.
   * regs->ax = RAX register = return value.
   * EXAMPLE: regs->ax = 0x0 → success.
   *          regs->ax = 0x1 → VM_FAULT_OOM.
   */
  pr_info("[PF_OBS] PID=%d FAULT_DONE ret=0x%lx\n", current->pid, regs->ax);
}

/*
 * pagefault_observer_init: Module load function.
 *
 * WHAT: Register kprobe on handle_mm_fault.
 *
 * STEPS:
 *   1. Set pre_handler and post_handler pointers.
 *   2. Call register_kprobe(&kp_fault).
 *   3. Kernel looks up symbol "handle_mm_fault" → address 0xffffffff812xxxxx.
 *   4. Kernel patches instruction at that address with INT3 (0xCC).
 *   5. When CPU hits INT3 → trap → our handler runs.
 *
 * NUMERICAL EXAMPLE:
 *   Original instruction at 0xffffffff81234560: 0x55 (push rbp).
 *   After register_kprobe: 0xCC (int3).
 *   CPU executes 0xffffffff81234560 → INT3 → handler_pre → restore 0x55 →
 * continue.
 *
 * RETURN:
 *   0 = success.
 *   -EINVAL = symbol not found.
 *   -ENOMEM = out of memory.
 */
static int __init pagefault_observer_init(void) {
  int ret;

  /* Register handle_mm_fault probe. */
  kp_fault.pre_handler = handler_pre;
  kp_fault.post_handler = handler_post;

  ret = register_kprobe(&kp_fault);
  if (ret < 0) {
    pr_err("[PF_OBS] register_kprobe(handle_mm_fault) failed: %d\n", ret);
    return ret;
  }

  /* Register vma_link probe. */
  kp_vma_link.pre_handler = vma_link_handler_pre;

  ret = register_kprobe(&kp_vma_link);
  if (ret < 0) {
    pr_err("[PF_OBS] register_kprobe(vma_link) failed: %d\n", ret);
    unregister_kprobe(&kp_fault);
    return ret;
  }

  /* Register __mmap_region probe. */
  kp_mmap_region.pre_handler = mmap_region_handler_pre;

  ret = register_kprobe(&kp_mmap_region);
  if (ret < 0) {
    pr_err("[PF_OBS] register_kprobe(__mmap_region) failed: %d\n", ret);
    unregister_kprobe(&kp_vma_link);
    unregister_kprobe(&kp_fault);
    return ret;
  }

  pr_info("[PF_OBS] Loaded. target_pid=%d\n", target_pid);
  pr_info("[PF_OBS] kprobe handle_mm_fault at 0x%px\n", kp_fault.addr);
  pr_info("[PF_OBS] kprobe vma_link at 0x%px\n", kp_vma_link.addr);
  pr_info("[PF_OBS] kprobe __mmap_region at 0x%px\n", kp_mmap_region.addr);
  return 0;
}

/*
 * pagefault_observer_exit: Module unload function.
 *
 * WHAT: Unregister kprobe.
 *
 * STEPS:
 *   1. Call unregister_kprobe(&kp_fault).
 *   2. Kernel restores original instruction (0x55 push rbp).
 *   3. Future calls to handle_mm_fault run without interception.
 */
static void __exit pagefault_observer_exit(void) {
  unregister_kprobe(&kp_mmap_region);
  unregister_kprobe(&kp_vma_link);
  unregister_kprobe(&kp_fault);
  pr_info("[PF_OBS] Unloaded.\n");
}

module_init(pagefault_observer_init);
module_exit(pagefault_observer_exit);

/*
 * ================================================================================
 * COMPLETE EXAMPLE:
 *
 * STEP 1: Compile.
 *   $ make modules
 *   → pagefault_observer.ko (12 KB).
 *
 * STEP 2: Load with PID filter.
 *   $ sudo insmod pagefault_observer.ko target_pid=1234
 *   dmesg: [PF_OBS] Loaded. Observing handle_mm_fault. target_pid=1234
 *   dmesg: [PF_OBS] kprobe at 0xffffffff81234560
 *
 * STEP 3: Run userspace program (PID 1234).
 *   $ ./mm_exercise_user
 *   → Allocates page at 0x7f0012345000.
 *   → Writes 0xAA to 0x7f0012345000.
 *   → CPU raises #PF.
 *
 * STEP 4: Observe dmesg.
 *   [PF_OBS] PID=1234 COMM=mm_exercise_use ADDR=0x7f0012345000 FLAGS=0x55
 * VMA=[0x7f0012340000-0x7f0012346000] [PF_OBS] PID=1234 FAULT_DONE ret=0x0
 *
 * STEP 5: Unload.
 *   $ sudo rmmod pagefault_observer
 *   dmesg: [PF_OBS] Unloaded.
 * ================================================================================
 */
