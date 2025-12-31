/*D01:AXIOM:Bit=0_or_1→D02:AXIOM:Byte=8_bits→D03:AXIOM:Page=4096_bytes=2^12→D04:AXIOM:RAM=Array[Byte]→D05:AXIOM:PFN=PhysAddr>>12→D06:AXIOM:struct_page=64_bytes_metadata_per_page→D07:AXIOM:vmemmap=0xffffea0000000000_on_x86_64→D08:AXIOM:_refcount=atomic_int_at_offset_52_in_struct_page→D09:PROBLEM:Multiple_users_of_same_page(mmap,sendfile,ioctl)→D10:SOLUTION:Reference_counting→when_refcount=0→page_can_be_freed*/
/*DRAW[struct_page@0xffffea0000000000:+0=flags(8B)|+8=_mapcount(4B)|+12=_refcount(4B)|+16=mapping(8B)|+24=index(8B)|+32=private(8B)|+40=lru.next(8B)|+48=lru.prev(8B)|+56=pad(8B)]*/
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/page_ref.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Refcount Manipulation Exercise");
/*DRAW[Stack:refcount_init()→page@RBP-8=???|pfn@RBP-16=???|ref@RBP-20=???]*/
static int __init refcount_init(void) {
  struct page *page;
  unsigned long pfn;
  int ref;
  printk(KERN_INFO "REFCOUNT:init\n");
  /*TODO01:CALCULATE:GFP_KERNEL=0xCC0=1100_1100_0000→bit6=__GFP_DIRECT_RECLAIM=can_wait→bit7=__GFP_KSWAPD_RECLAIM=wake_kswapd→bit10=__GFP_IO=allow_disk_io→bit11=__GFP_FS=allow_fs_operations→FILL:alloc_page(???)*/
  page = alloc_page(/* TODO: FILL GFP FLAGS */);
  if (!page) {
    printk(KERN_ERR "REFCOUNT:alloc_fail\n");
    return -12;
  }
  /*TODO02:DERIVE:page_to_pfn(page)→vmemmap=0xffffea0000000000→sizeof(struct_page)=64→formula:(page-vmemmap)/64→EXAMPLE:if_page=0xffffea0007a21f40→diff=0x7a21f40=128065344→pfn=128065344/64=2001021→FILL:pfn=???*/
  pfn = /* TODO: FILL FUNCTION CALL */;
  /*TODO03:DERIVE:page_ref_count(page)→accesses_page+52→reads_atomic_t→initial_value_after_alloc=???→FILL:ref=???*/
  ref = /* TODO: FILL FUNCTION CALL */;
  printk(KERN_INFO "REFCOUNT:alloc:page=%px,pfn=0x%lx,ref=%d\n", page, pfn,
         ref);
  /*DRAW[Before_get_page:page+52→_refcount=1]→TODO04:PREDICT:after_get_page→_refcount=???*/
  printk(KERN_INFO "REFCOUNT:before_get:ref=%d\n", page_ref_count(page));
  /*TODO05:CALL_get_page(page)→increments_refcount→VERIFY:ref_after=ref_before+1*/
  /* TODO: CALL get_page() HERE */
  printk(KERN_INFO "REFCOUNT:after_get:ref=%d\n", page_ref_count(page));
  /*DRAW[Before_put1:_refcount=2]→TODO06:PREDICT:after_put1→_refcount=???→freed=???*/
  printk(KERN_INFO "REFCOUNT:before_put1:ref=%d\n", page_ref_count(page));
  /*TODO07:CALL_put_page(page)→decrements_refcount→if_reaches_0→__free_page→page_goes_to_buddy*/
  /* TODO: CALL put_page() HERE */
  printk(KERN_INFO "REFCOUNT:after_put1:ref=%d\n", page_ref_count(page));
  /*DRAW[Before_put2:_refcount=1]→TODO08:PREDICT:after_put2→_refcount=???→freed=???*/
  printk(KERN_INFO "REFCOUNT:before_put2:ref=%d\n", page_ref_count(page));
  /*TODO09:CALL_put_page(page)→_refcount:1→0→page_freed→page_ptr=DANGLING→PREDICT:what_happens_if_we_call_put_page_again?*/
  /* TODO: CALL put_page() HERE */
  printk(KERN_INFO "REFCOUNT:after_put2:page_freed\n");
  /*F1:FAILURE_PREDICTION:If_we_call_put_page_again→_refcount=0-1=-1→VM_BUG_ON_PAGE(page_ref_count<=0)→kernel_panic_or_WARN*/
  /*TODO10:UNCOMMENT_LINE_BELOW_TO_TRIGGER_BUG→OBSERVE_dmesg→document_error_in_worksheet*/
  /*put_page(page);*/
  printk(KERN_INFO "REFCOUNT:done\n");
  return 0;
}
static void __exit refcount_exit(void) { printk(KERN_INFO "REFCOUNT:exit\n"); }
module_init(refcount_init);
module_exit(refcount_exit);
/*VIOLATION_CHECK:NEW_THINGS_INTRODUCED_WITHOUT_DERIVATION:*/
/*V1:page_ref_count→derived_in_TODO03*/
/*V2:get_page→derived_in_TODO05*/
/*V3:put_page→derived_in_TODO07*/
/*V4:offset_52→stated_in_D08→SOURCE:/usr/src/linux-headers-$(uname
 * -r)/include/linux/mm_types.h*/
/*USER_MUST_VERIFY:grep_-n_"_refcount"_/usr/src/linux-headers-6.14.0-37-generic/include/linux/mm_types.h*/
