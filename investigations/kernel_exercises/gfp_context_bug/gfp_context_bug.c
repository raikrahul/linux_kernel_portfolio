/*gfp_context_bug.c:DRAW[preempt_count():bits[0-7]=preempt_depth|bits[8-15]=softirq_count|bits[16-19]=hardirq_count|bits[20]=nmi→in_interrupt()=(preempt_count()&0x1FFF00)!=0→if_true→atomic_context→GFP_KERNEL=FORBIDDEN→must_use_GFP_ATOMIC]→DRAW[GFP_KERNEL=0xCC0=__GFP_RECLAIM|__GFP_IO|__GFP_FS→__GFP_RECLAIM=can_sleep→sleep_in_atomic=BUG]→DRAW[GFP_ATOMIC=0x0=no_sleep_flags→safe_in_interrupt]*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("GFP Context Bug Demo");
static struct timer_list my_timer;
/*DRAW[timer_callback:runs_in_softirq_context→preempt_count().bits[8-15]!=0→in_interrupt()=true→atomic_context→alloc_page(GFP_KERNEL)=BUG_sleeping_in_atomic]*/
static void timer_callback(struct timer_list *t)
{
    /*DRAW[BEFORE:in_interrupt()?→preempt_count()=0x????→bits[8-15]=softirq_count=?→if_softirq_count>0→in_softirq=true→atomic]*/
    struct page *page;
    unsigned long pfn;
    int in_atomic;
    /*TODO_USER:CALCULATE:preempt_count_bits→bits[8-15]=softirq→mask=0xFF00→preempt_count()&0xFF00=?→if_!=0→softirq_context*/
    in_atomic = in_interrupt();
    printk(KERN_INFO "GFP_CONTEXT:timer_callback:in_interrupt()=%ld,preempt_count()=0x%x\n", (long)in_atomic, preempt_count());
    /*DRAW[BUG_PATH:GFP_KERNEL=0xCC0→contains__GFP_RECLAIM=can_sleep→schedule()_called→BUG_scheduling_while_atomic→kernel_warning_or_panic]*/
    /*CORRECT_PATH:GFP_ATOMIC=0x0→no__GFP_RECLAIM→no_sleep→safe_in_interrupt*/
    /*TODO_USER:UNCOMMENT_ONE_LINE_AT_A_TIME_TO_SEE_DIFFERENCE:*/
    /*BUG_LINE:page = alloc_page(GFP_KERNEL);*/  /*←UNCOMMENT_TO_TRIGGER_BUG*/
    page = alloc_page(GFP_ATOMIC);              /*←SAFE_VERSION*/
    if (!page) {
        printk(KERN_ERR "GFP_CONTEXT:alloc_FAILED(expected_with_GFP_ATOMIC_under_memory_pressure)\n");
        return;
    }
    pfn = page_to_pfn(page);
    /*DRAW[SUCCESS:page@PFN=X→allocated_in_atomic_context→GFP_ATOMIC_worked→no_sleep_attempted]*/
    printk(KERN_INFO "GFP_CONTEXT:alloc_SUCCESS:pfn=0x%lx,used_GFP_ATOMIC\n", pfn);
    put_page(page);
    printk(KERN_INFO "GFP_CONTEXT:freed\n");
}
static int __init gfp_context_init(void)
{
    /*DRAW[init:process_context→preempt_count()=0→in_interrupt()=false→GFP_KERNEL=SAFE_HERE]*/
    printk(KERN_INFO "GFP_CONTEXT:__init:in_interrupt()=%ld,preempt_count()=0x%x\n", in_interrupt(), preempt_count());
    /*DRAW[timer_setup:arm_timer_to_fire_in_100ms→when_fires→timer_callback_runs_in_softirq→atomic_context]*/
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(100));
    printk(KERN_INFO "GFP_CONTEXT:timer_armed_100ms\n");
    return 0;
}
static void __exit gfp_context_exit(void)
{
    del_timer_sync(&my_timer);
    printk(KERN_INFO "GFP_CONTEXT:exit\n");
}
module_init(gfp_context_init);
module_exit(gfp_context_exit);
