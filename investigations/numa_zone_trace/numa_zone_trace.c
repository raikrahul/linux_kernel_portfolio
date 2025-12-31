/*L01:AXIOM:C_language_requires_#include_before_using_symbols→#include_copies_header_file_content_into_this_file*/
/*L02:AXIOM:Kernel_modules_require_MODULE_LICENSE→without_it_kernel_marks_module_as_tainted→restricts_symbol_access*/
/*L03:AXIOM:linux/module.h_provides:MODULE_LICENSE,MODULE_AUTHOR,MODULE_DESCRIPTION,module_init,module_exit→these_are_macros*/
/*L04:USING_L03:Include_linux/module.h_to_get_MODULE_LICENSE_macro*/
#include <linux/module.h>
/*L05:AXIOM:printk=kernel_printf→writes_to_ring_buffer→dmesg_reads_ring_buffer*/
/*L06:AXIOM:linux/kernel.h_provides_printk*/
/*L07:USING_L06:Include_linux/kernel.h_to_get_printk*/
#include <linux/kernel.h>
/*L08:AXIOM:__init=section_attribute→code_placed_in_.init.text_section→kernel_frees_this_section_after_boot→saves_RAM*/
/*L09:AXIOM:__exit=section_attribute→code_for_module_unload*/
/*L10:AXIOM:linux/init.h_provides___init_and___exit*/
/*L11:USING_L10:Include_linux/init.h_to_get___init*/
#include <linux/init.h>
/*L12:AXIOM:Page=4096_contiguous_bytes_of_physical_RAM→from_worksheet_line06*/
/*L13:AXIOM:PFN=Page_Frame_Number=physical_address÷4096→from_worksheet_line08*/
/*L14:AXIOM:struct_page=64_byte_structure_describing_one_physical_page→from_worksheet_line_implied*/
/*L15:AXIOM:linux/mm.h_provides:struct_page,page_to_pfn(),page_zone(),page_to_nid()*/
/*L16:USING_L15:Include_linux/mm.h_to_get_page_functions*/
#include <linux/mm.h>
/*L17:AXIOM:alloc_page(gfp)=allocates_one_page→returns_struct_page*_or_NULL*/
/*L18:AXIOM:GFP_KERNEL=0xCC0=allocation_flags_allowing_sleep→from_bootmem_trace_worksheet*/
/*L19:AXIOM:linux/gfp.h_provides:alloc_page,GFP_KERNEL*/
/*L20:USING_L19:Include_linux/gfp.h_to_get_alloc_page*/
#include <linux/gfp.h>
/*L21:AXIOM:NUMA=Non-Uniform_Memory_Access→multiple_CPU_sockets→each_socket_has_local_RAM→from_worksheet_line15*/
/*L22:AXIOM:Node=contiguous_RAM_attached_to_one_CPU_socket→from_worksheet_line13*/
/*L23:AXIOM:pg_data_t=structure_describing_one_NUMA_node→contains:node_start_pfn,node_spanned_pages,node_present_pages*/
/*L24:AXIOM:NODE_DATA(nid)=macro_returning_pg_data_t*_for_node_nid*/
/*L25:AXIOM:Zone=RAM_region_with_address_constraints:DMA<16MB,DMA32<4GB,Normal≥4GB→from_worksheet_lines27-43*/
/*L26:AXIOM:struct_zone=structure_describing_one_zone→contains:zone_start_pfn,spanned_pages,present_pages,name*/
/*L27:AXIOM:linux/mmzone.h_provides:pg_data_t,struct_zone,NODE_DATA,pageblock_order,pageblock_nr_pages*/
/*L28:USING_L27:Include_linux/mmzone.h_to_get_node_and_zone_structures*/
#include <linux/mmzone.h>
/*L29:AXIOM:for_each_online_node(nid)=macro→iterates_over_online_NUMA_nodes→from_worksheet_line55-57*/
/*L30:AXIOM:num_online_nodes()=function_returning_count_of_online_nodes*/
/*L31:AXIOM:linux/nodemask.h_provides:for_each_online_node,num_online_nodes*/
/*L32:USING_L31:Include_linux/nodemask.h_to_get_node_iteration_macros*/
#include <linux/nodemask.h>
/*L33:USING_L02,L03:MODULE_LICENSE("GPL")→tells_kernel_this_module_is_GPL_licensed*/
MODULE_LICENSE("GPL");
/*L34:USING_L03:MODULE_AUTHOR→metadata_visible_in_modinfo*/
MODULE_AUTHOR("User");
/*L35:USING_L03:MODULE_DESCRIPTION→metadata_visible_in_modinfo*/
MODULE_DESCRIPTION("NUMA Zone Trace");
/*L36:DEFINITION:init_function=called_when_insmod_loads_module→returns_0_on_success,negative_on_error*/
/*L37:USING_L08,L36:__init_marks_function_for_init_section*/
static int __init numa_zone_init(void)
{
    /*L38:AXIOM:Local_variables_live_on_stack→stack_grows_downward_on_x86_64→rbp=base_pointer*/
    /*L39:USING_L14:struct_page*=pointer=8_bytes_on_x86_64*/
    struct page *page;
    /*L40:USING_L13:unsigned_long=8_bytes→stores_PFN_value*/
    unsigned long pfn;
    /*L41:USING_L22:int=4_bytes→stores_node_id→this_machine:nid∈{0}*/
    int nid;
    /*L42:USING_L26:struct_zone*=pointer=8_bytes→points_to_zone_structure*/
    struct zone *zone;
    /*L43:USING_L05:printk_with_KERN_INFO_logs_to_dmesg*/
    printk(KERN_INFO "NUMA_ZONE:init\n");
    /*L44:USING_L30:Call_num_online_nodes()→returns_count*/
    /*L45:AXIOM:This_machine_has_1_node→from_worksheet_line10→expected_output:1*/
    /*L46:TODO_USER:PREDICT_before_running:num_online_nodes()=___*/
    printk(KERN_INFO "NUMA_ZONE:nodes=%d\n", num_online_nodes());
    /*L47:USING_L29:for_each_online_node(nid)→loop_iterates_nid=0_only_on_this_machine*/
    for_each_online_node(nid) {
        /*L48:USING_L24:NODE_DATA(0)→returns_pg_data_t*_for_node_0*/
        pg_data_t *pgdat = NODE_DATA(nid);
        /*L49:USING_L23:pgdat->node_start_pfn=first_PFN_in_node*/
        /*L50:USING_L23:pgdat->node_spanned_pages=total_PFNs_including_holes*/
        /*L51:USING_L23:pgdat->node_present_pages=usable_PFNs_excluding_holes*/
        /*L52:TODO_USER:PREDICT:node_start_pfn=___,spanned≈___,present≈___*/
        printk(KERN_INFO "NUMA_ZONE:node[%d]:start=%lu,span=%lu,present=%lu\n",
               nid, pgdat->node_start_pfn, pgdat->node_spanned_pages, pgdat->node_present_pages);
    }
    /*L53:USING_L17,L18:alloc_page(GFP_KERNEL)→allocates_1_page→returns_struct_page**/
    page = alloc_page(GFP_KERNEL);
    /*L54:AXIOM:NULL=0x0=invalid_pointer→alloc_page_returns_NULL_on_failure*/
    if (!page) {
        printk(KERN_ERR "NUMA_ZONE:alloc_fail\n");
        /*L55:AXIOM:ENOMEM=12→defined_in_errno.h→-ENOMEM=-12=out_of_memory_error*/
        return -ENOMEM;
    }
    /*L56:USING_L15:page_to_pfn(page)→calculates:(page-vmemmap)/64→returns_PFN*/
    /*L57:TODO_USER:After_insmod→note_pfn_value→CALCULATE:pfn×4096=physical_address*/
    pfn = page_to_pfn(page);
    /*L58:USING_L15:page_to_nid(page)→extracts_node_id_from_page->flags*/
    /*L59:TODO_USER:PREDICT:nid=___→this_machine_has_1_node→nid=0*/
    nid = page_to_nid(page);
    /*L60:USING_L15:page_zone(page)→extracts_zone_id_from_page->flags→returns_struct_zone**/
    zone = page_zone(page);
    /*L61:USING_L26:zone->name=string:"DMA"_or_"DMA32"_or_"Normal"*/
    /*L62:USING_worksheet_L46:Zone_determined_by:PFN<4096→DMA,4096≤PFN<1048576→DMA32,PFN≥1048576→Normal*/
    /*L63:TODO_USER:PREDICT:If_pfn≈1000000_or_larger→zone="Normal"*/
    printk(KERN_INFO "NUMA_ZONE:pfn=0x%lx(%lu),nid=%d,zone=%s\n", pfn, pfn, nid, zone->name);
    /*L64:USING_L26:zone->zone_start_pfn=first_PFN_in_zone*/
    /*L65:TODO_USER:VERIFY:If_zone="Normal"→start_pfn=1048576(from_worksheet_L44)*/
    printk(KERN_INFO "NUMA_ZONE:%s:start=%lu,span=%lu\n", zone->name, zone->zone_start_pfn, zone->spanned_pages);
    /*L66:USING_L27:pageblock_order=compile-time_constant→x86_64=9*/
    /*L67:USING_L27:pageblock_nr_pages=2^pageblock_order→from_worksheet_L60:2^9=512*/
    /*L68:TODO_USER:VERIFY:pageblock_order=9→pageblock_nr_pages=512*/
    printk(KERN_INFO "NUMA_ZONE:pageblock_order=%d,nr_pages=%lu\n", pageblock_order, pageblock_nr_pages);
    /*L69:USING_L12:PAGE_SIZE=4096→physical_address=pfn×4096=pfn<<12*/
    /*L70:TODO_USER:CALCULATE:Take_pfn_from_output→multiply_by_4096→verify_hex_value*/
    printk(KERN_INFO "NUMA_ZONE:phys=0x%llx\n", (unsigned long long)pfn * PAGE_SIZE);
    /*L71:AXIOM:put_page(page)→decrements_refcount→if_refcount_reaches_0→page_freed*/
    /*L72:USING_L71:This_page_was_allocated_with_refcount=1→put_page→refcount=0→freed*/
    put_page(page);
    printk(KERN_INFO "NUMA_ZONE:done\n");
    return 0;
}
/*L73:USING_L09:__exit_marks_function_for_exit_section*/
static void __exit numa_zone_exit(void)
{
    printk(KERN_INFO "NUMA_ZONE:exit\n");
}
/*L74:USING_L03:module_init()→registers_init_function→kernel_calls_it_on_insmod*/
module_init(numa_zone_init);
/*L75:USING_L03:module_exit()→registers_exit_function→kernel_calls_it_on_rmmod*/
module_exit(numa_zone_exit);
/*L76:---VIOLATION_CHECK---*/
/*L77:NEW_THINGS_INTRODUCED_WITHOUT_FULL_DERIVATION:*/
/*L78:V1:L14:sizeof(struct_page)=64→stated_without_showing_pahole_or_source*/
/*L79:V2:L23:pg_data_t_fields→stated_without_showing_struct_definition_from_mmzone.h*/
/*L80:V3:L26:struct_zone_fields→stated_without_showing_struct_definition*/
/*L81:V4:L54:NULL=0x0→stated_without_deriving_from_stddef.h*/
/*L82:V5:L56:vmemmap_value→referenced_but_not_computed_for_this_machine*/
/*L83:V6:L66:pageblock_order=9→stated_as_axiom_but_not_verified_via_grep*/
/*L84:FIX_REQUIRED:User_must_verify_each_violation_by_running_appropriate_command*/
