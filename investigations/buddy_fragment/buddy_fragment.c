/*buddy_fragment.c:DRAW[/proc/buddyinfo:Normal:761|628|345|167|306|194|91|31|20|11|0→order0=761blocks×1page=761pages|order1=628×2=1256|order2=345×4=1380|order3=167×8=1336|order10=0×1024=0pages]→DRAW[PROBLEM:alloc_pages(GFP_KERNEL,10)→order10=0blocks→split_from_order11?→MAX_ORDER=11→no_order11→FAIL_return_NULL]→DRAW[FRAGMENTATION:nr_free_pages=184010→enough_pages→but_not_contiguous→order10_needs_1024_contiguous→none_exist]*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mm.h>
#include <linux/gfp.h>
MODULE_LICENSE("GPL");
MODULE_AUTHOR("User");
MODULE_DESCRIPTION("Buddy Fragmentation Demo");
static int __init buddy_frag_init(void)
{
    /*DRAW[stack:buddy_frag_init()→local_vars:pages[4]@rbp-32=8bytes×4=32bytes|i@rbp-36=4bytes|order3_page@rbp-44=8bytes]*/
    struct page *pages[4];
    struct page *order3_page;
    int i;
    unsigned long pfn;
    printk(KERN_INFO "BUDDY_FRAG:__init\n");
    /*DRAW[BEFORE:cat/proc/buddyinfo→Normal:order0=761|order3=167]→STEP1:alloc_pages(GFP_KERNEL,3)→order3=167blocks→take_one→order3=166→page_returned@PFN=X→refcount=1*/
    /*TODO_USER:CALCULATE:order3=2^3=___pages×4096=___bytes*/
    /*TODO_USER:FILL:order3_bytes=___*/
    order3_page = alloc_pages(GFP_KERNEL, 3);
    if (!order3_page) {
        printk(KERN_ERR "BUDDY_FRAG:order3_alloc_FAILED→order3_blocks=0→fragmented\n");
        return -ENOMEM;
    }
    pfn = page_to_pfn(order3_page);
    /*DRAW[order3_page@pfn=X→covers_PFNs:X,X+1,X+2,X+3,X+4,X+5,X+6,X+7→8_contiguous_pages→phys_range:X×4096_to_(X+8)×4096-1]*/
    printk(KERN_INFO "BUDDY_FRAG:order3_alloc:pfn=0x%lx(%lu),covers_pfn_%lu_to_%lu,bytes=%lu\n", 
           pfn, pfn, pfn, pfn+7, 8UL*PAGE_SIZE);
    /*DRAW[FRAGMENTATION_DEMO:alloc_4×order0_pages→each_order0=1page→scattered_across_memory→not_contiguous]*/
    for (i = 0; i < 4; i++) {
        /*TODO_USER:PREDICT:after_this_loop→order0_blocks=761-4=___*/
        pages[i] = alloc_page(GFP_KERNEL);
        if (!pages[i]) {
            printk(KERN_ERR "BUDDY_FRAG:order0_alloc_%d_FAILED\n", i);
            goto cleanup;
        }
        printk(KERN_INFO "BUDDY_FRAG:order0[%d]:pfn=0x%lx\n", i, page_to_pfn(pages[i]));
    }
    /*DRAW[AFTER_ALLOCS:order0:761→757(-4)|order3:167→166(-1)|scattered_4_single_pages_across_memory]*/
    /*DRAW[FREE_ORDER0_PAGES:put_page×4→each_page→buddy_check→if_buddy_free→merge→else→add_to_order0_list]*/
    for (i = 0; i < 4; i++) {
        /*TODO_USER:TRACE:put_page(pages[i])→find_buddy→buddy_pfn=pfn_XOR_1→is_buddy_in_free_area[0]?→merge_or_not?*/
        printk(KERN_INFO "BUDDY_FRAG:freeing_order0[%d]:pfn=0x%lx→buddy_pfn=0x%lx\n", 
               i, page_to_pfn(pages[i]), page_to_pfn(pages[i]) ^ 1);
        put_page(pages[i]);
    }
    /*DRAW[AFTER_FREE_ORDER0:check_buddyinfo→did_order0_increase_by_4?→or_did_merges_happen?→merges_increase_higher_orders]*/
    /*DRAW[FREE_ORDER3:__free_pages(order3_page,3)→buddy_check→buddy_pfn=pfn_XOR_8→is_buddy_free?→merge_to_order4_or_stay_order3]*/
    printk(KERN_INFO "BUDDY_FRAG:freeing_order3:pfn=0x%lx→buddy_pfn=0x%lx\n", 
           pfn, pfn ^ 8);
    __free_pages(order3_page, 3);
    printk(KERN_INFO "BUDDY_FRAG:init_complete→check_/proc/buddyinfo\n");
    return 0;
cleanup:
    for (i = i - 1; i >= 0; i--)
        put_page(pages[i]);
    __free_pages(order3_page, 3);
    return -ENOMEM;
}
static void __exit buddy_frag_exit(void)
{
    printk(KERN_INFO "BUDDY_FRAG:exit\n");
}
module_init(buddy_frag_init);
module_exit(buddy_frag_exit);
