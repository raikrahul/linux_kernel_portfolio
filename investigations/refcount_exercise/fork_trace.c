/*==========================================================================*/
/*                    KPROBE TRACER FOR kernel_clone                        */
/*==========================================================================*/
/*01.AXIOM:byte=8_bits→memory=array_of_bytes→each_byte_has_address_0,1,2,...*/
/*02.USING_01:function=sequence_of_bytes(machine_code)_at_specific_address*/
/*03.LIVE_DATA:cat_/proc/kallsyms|grep_kernel_clone→ffffffffaf785520_T_kernel_clone*/
/*04.USING_03:kernel_clone_function_starts_at_RAM_address_0xffffffffaf785520*/
/*05.AXIOM:kprobe_replaces_first_byte_with_INT3(0xCC)→CPU_traps_to_handler*/

#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/module.h>
#include <linux/sched.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fork Trace Step 1: kernel_clone only");

/*==========================================================================*/
/*06.AXIOM:struct_kprobe_has_field_.symbol_name=pointer_to_string*/
/*07.USING_06:kernel_uses_string_to_lookup_address_in_kallsyms*/
/*08.LIVE_DATA:you_need_string_"kernel_clone"_which_is_12_chars_+_null=13_bytes*/
/*09.CALCULATION:"kernel_clone"→k-e-r-n-e-l-_-c-l-o-n-e-\0→count:12+1=13*/

/*FORWARD_DECLARE_SO_STRUCT_CAN_REFERENCE:*/
static int my_kernel_clone_handler(struct kprobe *p, struct pt_regs *regs);
static void my_kernel_clone_post(struct kprobe *p, struct pt_regs *regs,
                                 unsigned long flags);

static struct kprobe kp_kernel_clone = {
    /*TODO01:WRITE_THE_STRING_LITERAL_FOR_SYMBOL_NAME:*/
    /*DATA:the_function_name_is_kernel_clone*/
    /*DATA:string_literal_syntax_in_C_is_double_quotes_around_text*/
    /*YOU_WRITE_BELOW:*/
    .symbol_name = "kernel_clone",
    .pre_handler = my_kernel_clone_handler, /*runs_BEFORE_function*/
    .post_handler = my_kernel_clone_post,   /*runs_AFTER_function_returns*/

};

/*==========================================================================*/
/*10.AXIOM:pre_handler_is_called_BEFORE_target_function_runs*/
/*11.AXIOM:handler_receives_struct_pt_regs*_containing_all_CPU_registers*/
/*12.LIVE_DATA:pt_regs_field_"di"_contains_RDI_register_value*/
/*13.LIVE_DATA:for_kernel_clone(args),_RDI=args_pointer=0xffffcc569367bbd8(example)*/
/*14.AXIOM:current_is_pointer_to_task_struct_of_running_process*/
/*15.LIVE_DATA:current->comm="race_test_user"(char[16])*/
/*16.LIVE_DATA:current->pid=236817(int,4_bytes)*/

static int my_kernel_clone_handler(struct kprobe *p, struct pt_regs *regs) {
  /*17.AXIOM:pre_handler_runs_at_T2_BEFORE_kernel_clone_executes*/
  /*18.DATA:at_this_moment_child_DOES_NOT_EXIST_yet*/
  /*19.DATA:regs->di=pointer_to_kernel_clone_args=0xffffcc5687657b78*/
  /*20.DATA:regs->sp=RSP=stack_pointer,RAM[RSP]=return_address*/
  /*21.AXIOM:%pS_prints_address_as_symbol+offset(e.g."__do_sys_clone+0x5f")*/
  /*22.AXIOM:task_pt_regs(current)=pt_regs_saved_at_syscall_entry=USERSPACE_regs*/

  unsigned long return_addr = *(unsigned long *)regs->sp;
  struct pt_regs *user_regs = task_pt_regs(current);

  printk(KERN_INFO "FORK_TRACE:[%s:%d] kernel_clone PRE (args=%lx)\n",
         current->comm, current->pid, regs->di);
  printk(KERN_INFO "FORK_TRACE:  kernel caller: %pS\n", (void *)return_addr);
  printk(KERN_INFO
         "FORK_TRACE:  user RIP: 0x%lx (addr2line -e <binary> 0x%lx)\n",
         user_regs->ip, user_regs->ip);
  return 0;
}

/*==========================================================================*/
/*20.AXIOM:post_handler_runs_AFTER_function_returns*/
/*21.DATA:at_this_moment_child_EXISTS_but_may_not_be_running_yet*/
/*22.DATA:regs->ax=RAX=return_value_of_kernel_clone=child_PID_or_error*/
/*23.EXAMPLE:if_child_PID=239494→regs->ax=239494*/
static void my_kernel_clone_post(struct kprobe *p, struct pt_regs *regs,
                                 unsigned long flags) {
  /*24.DATA:current_is_still_PARENT_because_we're_in_parent's_syscall_return_path*/
  /*25.DATA:regs->ax_contains_child_PID_(positive)_or_error_(negative)*/
  printk(KERN_INFO
         "FORK_TRACE:[%s:%d] kernel_clone POST (ret=%ld, child_pid=%ld)\n",
         current->comm, current->pid, regs->ax, regs->ax);
}

/*==========================================================================*/
static int __init fork_trace_init(void) {
  int ret;
  printk(KERN_INFO "FORK_TRACE:Loading probe for kernel_clone...\n");

  /*HANDLERS_ALREADY_ASSIGNED_IN_STRUCT_INITIALIZER_ABOVE*/

  /*TODO04:CALL_REGISTER_KPROBE:*/
  /*DATA:function_name_is_register_kprobe*/
  /*DATA:argument_is_address_of_kp_kernel_clone→use_&_operator*/
  /*SYNTAX:function_name(&variable)*/
  /*YOU_WRITE_BELOW:*/
  ret = register_kprobe(&kp_kernel_clone);

  if (ret < 0) {
    printk(KERN_ERR "FORK_TRACE:register_kprobe failed: %d\n", ret);
    return ret;
  }

  printk(KERN_INFO "FORK_TRACE:Probe registered at %px\n",
         kp_kernel_clone.addr);
  return 0;
}

/*==========================================================================*/
static void __exit fork_trace_exit(void) {
  /*TODO05:CALL_UNREGISTER_KPROBE:*/
  /*DATA:function_name_is_unregister_kprobe*/
  /*DATA:argument_is_address_of_kp_kernel_clone→use_&_operator*/
  /*SYNTAX:function_name(&variable);*/
  /*YOU_WRITE_BELOW:*/
  unregister_kprobe(&kp_kernel_clone);
  printk(KERN_INFO "FORK_TRACE:Probe removed.\n");
}

module_init(fork_trace_init);
module_exit(fork_trace_exit);
