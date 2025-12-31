/*01.AXIOM:bit=0_or_1→02.USING_01:byte=8bits→03.USING_02:page=4096bytes→04.USING_03:RAM=page_array→05.USING_04:PFN=index_into_array→06.USING_05:struct_page=metadata_per_PFN→07.USING_06:refcount=integer_in_struct_page_tracking_users*/
/*08.DRAW[RAM:|PFN0|PFN1|PFN2|...|PFN3944068|]→each_box=4096bytes→09.DRAW[vmemmap:|struct_page[0]|struct_page[1]|...|]→each_box=64bytes→10.struct_page[N]_describes_PFN_N*/
#include <linux/gfp.h> /*11.SOURCE:/usr/src/linux-headers-6.14.0-37-generic/include/linux/gfp_types.h→line262:GFP_KERNEL=___GFP_RECLAIM|___GFP_IO|___GFP_FS=0x400|0x40|0x80=0x4C0↑TRAP:old_value_was_0xCC0_check_your_kernel_version*/
#include <linux/init.h> /*12.SOURCE:provides___init_attribute→code_placed_in_.init.text→freed_after_boot*/
#include <linux/kernel.h> /*13.SOURCE:provides_printk→writes_to_kernel_ring_buffer→read_via_dmesg*/
#include <linux/mm.h> /*14.SOURCE:provides_page_to_pfn,page_ref_count→line1923:page_to_pfn→line2301:page_ref_count*/
#include <linux/module.h> /*15.SOURCE:provides_MODULE_LICENSE,module_init,module_exit*/
#include <linux/page_ref.h>/*16.SOURCE:provides_get_page,put_page→get_page=atomic_inc(&page->_refcount)→put_page=atomic_dec_and_test*/
MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Refcount Manipulation Exercise");
static int __init refcount_init(void) {
  struct page *
      page; /*17.DRAW[stack:page@RBP-8=0x????????????????]→8bytes→points_to_struct_page_in_vmemmap*/
  unsigned long
      pfn; /*18.DRAW[stack:pfn@RBP-16=0x????????????????]→8bytes→will_hold_page_frame_number_0..3944068*/
  int ref; /*19.DRAW[stack:ref@RBP-20=0x????????]→4bytes→will_hold_refcount_value_1,2,3...*/
  printk(KERN_INFO "REFCOUNT:init\n");
  /*TODO01:YOU_MUST_DERIVE_GFP_KERNEL_VALUE:grep_-n_"GFP_KERNEL"_/usr/src/linux-headers-6.14.0-37-generic/include/linux/gfp_types.h→find_line_number→read_definition→expand_macros→CALCULATE:___GFP_RECLAIM=bit10=0x400→___GFP_IO=bit6=0x40→___GFP_FS=bit7=0x80→GFP_KERNEL=0x400|0x40|0x80=___→FILL_BELOW:*/
  page = alloc_page(/* TODO: DERIVE VALUE FROM GREP, WRITE HERE */);
  if (!page) {
    printk(KERN_ERR "REFCOUNT:alloc_fail\n");
    return -12;
  }
  /*TODO02:YOU_MUST_DERIVE_page_to_pfn:AXIOM:vmemmap_base=0xffffea0000000000→AXIOM:sizeof(struct_page)=64→FORMULA:(page_ptr-vmemmap_base)/64=pfn→EXAMPLE:if_page=0xffffea0007a21f40→step1:0xffffea0007a21f40-0xffffea0000000000=0x7a21f40→step2:0x7a21f40=128065344_decimal→step3:128065344/64=2001021→∴pfn=2001021→FILL_BELOW:*/
  pfn = /* TODO: WRITE FUNCTION NAME HERE: ???(page) */;
  /*TODO03:YOU_MUST_DERIVE_page_ref_count:AXIOM:alloc_page_calls_set_page_refcounted→set_page_refcounted_calls_atomic_set(&page->_refcount,1)→∴after_alloc_refcount=1→VERIFY:grep_"set_page_refcounted"_/usr/src/linux-headers-6.14.0-37-generic/include/linux/mm.h→FILL_BELOW:*/
  ref = /* TODO: WRITE FUNCTION NAME HERE: ???(page) */;
  printk(KERN_INFO "REFCOUNT:alloc:page=%px,pfn=0x%lx,ref=%d\n", page, pfn,
         ref);
  /*TODO04:DRAW_BEFORE_get_page:[struct_page@vmemmap+pfn*64:+0=flags|+52=_refcount=1|+56=...]→get_page=atomic_inc→1+1=2→DRAW_AFTER:[_refcount=2]*/
  printk(KERN_INFO "REFCOUNT:before_get:ref=%d\n", page_ref_count(page));
  /*TODO05:YOU_MUST_CALL_get_page:SOURCE:linux/page_ref.h→get_page(page)_increments_refcount→FILL_BELOW:*/
  /* TODO: WRITE FUNCTION CALL HERE: ???(page); */
  printk(KERN_INFO "REFCOUNT:after_get:ref=%d\n", page_ref_count(page));
  /*TODO06:DRAW_BEFORE_first_put_page:[_refcount=2]→put_page=atomic_dec_and_test→2-1=1→test:1==0?→✗→page_NOT_freed→DRAW_AFTER:[_refcount=1]*/
  printk(KERN_INFO "REFCOUNT:before_put1:ref=%d\n", page_ref_count(page));
  /*TODO07:YOU_MUST_CALL_put_page:SOURCE:linux/page_ref.h→put_page(page)_decrements_refcount→if_reaches_0_frees_page→FILL_BELOW:*/
  /* TODO: WRITE FUNCTION CALL HERE: ???(page); */
  printk(KERN_INFO "REFCOUNT:after_put1:ref=%d\n", page_ref_count(page));
  /*TODO08:DRAW_BEFORE_second_put_page:[_refcount=1]→put_page=atomic_dec_and_test→1-1=0→test:0==0?→✓→__free_page_called→page_returned_to_buddy_freelist*/
  printk(KERN_INFO "REFCOUNT:before_put2:ref=%d\n", page_ref_count(page));
  /*TODO09:YOU_MUST_CALL_put_page_AGAIN:THIS_FREES_THE_PAGE:after_this_page_ptr_is_DANGLING→accessing_freed_page=UNDEFINED_BEHAVIOR→FILL_BELOW:*/
  /* TODO: WRITE FUNCTION CALL HERE: ???(page); */
  printk(KERN_INFO "REFCOUNT:after_put2:page_freed\n");
  /*TODO10:TRAP:IF_YOU_UNCOMMENT_BELOW→put_page_on_freed_page→refcount=0→0-1=-1→VM_BUG_ON_PAGE(page_ref_count(page)<=0)→kernel_panic_or_WARN→UNCOMMENT_TO_TRIGGER_BUG:*/
  /*put_page(page);*/
  printk(KERN_INFO "REFCOUNT:done\n");
  return 0;
}
static void __exit refcount_exit(void) { printk(KERN_INFO "REFCOUNT:exit\n"); }
module_init(refcount_init);
module_exit(refcount_exit);
/*FAILURE_PREDICTIONS:F1:forgot_to_call_get_page→refcount_stays_1→only_need_one_put_page→second_put_page_causes_underflow|F2:called_get_page_twice→refcount=3→need_three_put_pages→if_only_two→memory_leak|F3:wrong_GFP_flag→GFP_ATOMIC_in_process_context→works_but_wastes_reserves|F4:forgot_to_check_NULL→alloc_page_returns_NULL_under_memory_pressure→crash*/
/*VIOLATION_CHECK:NEW_THINGS_WITHOUT_DERIVATION:atomic_inc_not_derived→USER_MUST_RUN:grep_"atomic_inc"_in_asm_headers*/
