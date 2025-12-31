/*bootmem_trace.c:DRAW[RAM:16154906624bytes→pages:3944069→vmemmap:0xffffea0000000000→struct_page[0..3944068]→each:64bytes]→DRAW[zones:DMA(PFN<4096)→DMA32(4096≤PFN<1048576)→Normal(PFN≥1048576)]→DRAW[GFP_KERNEL:BIT(6)|BIT(7)|BIT(10)|BIT(11)=0x40|0x80|0x400|0x800=0xCC0]*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
#include <linux/page_ref.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Bootmem Trace");
static int __init bootmem_init(void)
{
    /*DRAW[stack:bootmem_init()→local_vars:page_ptr(8bytes@rbp-8)=???|pfn(8bytes@rbp-16)=???|phys(8bytes@rbp-24)=???|ref(4bytes@rbp-28)=???]*/
    struct page *page_ptr;
    unsigned long pfn;
    phys_addr_t phys;
    int ref;
    printk(KERN_INFO "BOOTMEM_TRACE:__init\n");
    /*DRAW[before_alloc:buddy_allocator→zone_Normal→free_area[0]→nr_free=42261(from/proc/buddyinfo)→first_free_page@PFN=?]→alloc_page(GFP_KERNEL=0xCC0)→__alloc_pages(gfp=0xCC0,order=0,node=-1)→get_page_from_freelist()→rmqueue(zone=Normal,order=0)→__rmqueue_smallest()→list_del(page)→nr_free:42261→42260→set_page_refcounted(page)→atomic_set(&page->_refcount,1)→return page*/
    page_ptr = alloc_page(GFP_KERNEL);
    /*DRAW[after_alloc:page_ptr=0xffffea0007a21f40(example)→page_ptr[0..63bytes]:+0=flags(8bytes)=0x0000000000010000|+8=_refcount(4bytes)=1|+12=_mapcount(4bytes)=-1|+16=mapping(8bytes)=NULL|+24=index(8bytes)=0|+32=private(8bytes)=0|+40=_last_cpupid(4bytes)=0|+44..63=pad]*/
    if (!page_ptr) { printk(KERN_ERR "BOOTMEM_TRACE:alloc_failed\n"); return -ENOMEM; }
    /*DRAW[page_to_pfn:page_ptr=0xffffea0007a21f40→vmemmap=0xffffea0000000000→diff=0xffffea0007a21f40-0xffffea0000000000=0x7a21f40=128065344→pfn=128065344/64=2001021]→page_to_pfn(page_ptr)→returns(page_ptr-vmemmap)/sizeof(struct_page)=(0xffffea0007a21f40-0xffffea0000000000)/64=2001021*/
    pfn = page_to_pfn(page_ptr);
    /*DRAW[pfn_to_phys:pfn=2001021→PAGE_SIZE=4096=2^12→phys=2001021×4096→step1:2001021×4000=8004084000→step2:2001021×96=192098016→step3:8004084000+192098016=8196182016=0x1e887d000]→phys=(phys_addr_t)pfn*PAGE_SIZE=2001021×4096=8196182016=0x1e887d000*/
    phys = (phys_addr_t)pfn * PAGE_SIZE;
    /*DRAW[page_ref_count:page_ptr=0xffffea0007a21f40→page_ptr+8=0xffffea0007a21f48→_refcount@0xffffea0007a21f48→atomic_read(&_refcount)→.counter=1]→page_ref_count(page_ptr)→returns atomic_read(&page->_refcount)=1*/
    ref = page_ref_count(page_ptr);
    /*DRAW[zone_check:pfn=2001021→DMA_boundary=4096→DMA32_boundary=1048576→2001021<4096?✗→2001021<1048576?✗→2001021≥1048576?✓→zone=Normal]*/
    printk(KERN_INFO "BOOTMEM_TRACE:page=%px,pfn=0x%lx(%lu),phys=0x%llx,ref=%d,zone=%s\n", page_ptr, pfn, pfn, phys, ref, pfn >= 1048576 ? "Normal" : (pfn >= 4096 ? "DMA32" : "DMA"));
    /*DRAW[before_get_page:page_ptr+8→_refcount=1]→get_page(page_ptr)→page_ref_inc(page)→atomic_inc(&page->_refcount)→lock_cmpxchg→_refcount:1→1+1=2→DRAW[after_get_page:_refcount=2]*/
    printk(KERN_INFO "BOOTMEM_TRACE:before_get:ref=%d\n", page_ref_count(page_ptr));
    get_page(page_ptr);
    printk(KERN_INFO "BOOTMEM_TRACE:after_get:ref=%d(expect2)\n", page_ref_count(page_ptr));
    /*DRAW[before_put1:_refcount=2]→put_page(page_ptr)→page_ref_dec_and_test(page)→atomic_dec_and_test(&page->_refcount)→lock_decl→_refcount:2→2-1=1→test:1==0?✗→return0→page_NOT_freed→DRAW[after_put1:_refcount=1]*/
    printk(KERN_INFO "BOOTMEM_TRACE:before_put1:ref=%d\n", page_ref_count(page_ptr));
    put_page(page_ptr);
    printk(KERN_INFO "BOOTMEM_TRACE:after_put1:ref=%d(expect1)\n", page_ref_count(page_ptr));
    /*DRAW[before_put2:_refcount=1]→put_page(page_ptr)→atomic_dec_and_test→lock_decl→_refcount:1→1-1=0→test:0==0?✓→return1→__page_cache_release(page)→free_unref_page(page)→free_unref_page_commit()→list_add(&page->lru,&pcp->lists[migratetype])→page_returned_to_pcp_freelist→DRAW[after_put2:_refcount=0→page_freed→page_ptr=DANGLING_POINTER→any_access=UNDEFINED_BEHAVIOR]*/
    printk(KERN_INFO "BOOTMEM_TRACE:before_put2:ref=%d\n", page_ref_count(page_ptr));
    put_page(page_ptr);
    printk(KERN_INFO "BOOTMEM_TRACE:after_put2:page_freed(ref_read_is_UB)\n");
    /*DRAW[before_put3_BUG:page_freed→_refcount=0(or_reused_by_kernel_with_refcount=1)→put_page(page_ptr)→atomic_dec_and_test→_refcount:0-1=-1→VM_BUG_ON_PAGE(page_ref_count(page)<=0,page)→-1<=0?✓→BUG()→kernel_panic_or_WARN→dmesg_shows:page:0xffffea0007a21f40_refcount:-1_mapcount:-1→DRAW[after_put3:_refcount=-1→CORRUPTION→if_page_reallocated_we_decremented_WRONG_PAGE_refcount]]*/
    /*UNCOMMENT_TO_TRIGGER_BUG:put_page(page_ptr);*/
    printk(KERN_INFO "BOOTMEM_TRACE:BUG_LINE_COMMENTED→uncomment_line_above_to_trigger_refcount_underflow\n");
    printk(KERN_INFO "BOOTMEM_TRACE:init_complete\n");
    return 0;
}
static void __exit bootmem_exit(void) { printk(KERN_INFO "BOOTMEM_TRACE:exit\n"); }
module_init(bootmem_init);
module_exit(bootmem_exit);
