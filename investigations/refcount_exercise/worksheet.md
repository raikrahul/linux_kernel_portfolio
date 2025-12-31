# Refcount Exercise Worksheet

## AXIOMS (SOURCES)
01. Bit=0_or_1→SOURCE:hardware_transistor_state
02. Byte=8_bits→SOURCE:x86_architecture_definition
03. Page=4096_bytes=2^12=0x1000→SOURCE:x86_MMU_page_size
04. RAM=Array[Byte]→SOURCE:physical_memory_model
05. PFN=PhysAddr>>12=PhysAddr/4096→SOURCE:kernel_mm_convention
06. struct_page=64_bytes→SOURCE:`pahole -C page vmlinux`
07. vmemmap=0xffffea0000000000→SOURCE:arch/x86/include/asm/page_64_types.h
08. _refcount=atomic_t_at_offset→SOURCE:include/linux/mm_types.h

## DERIVATION CHAIN (USER FILLS)
09. FILL:offset_of_refcount_in_struct_page=___bytes→RUN:grep_-n_"_refcount"_/usr/src/linux-headers-6.14.0-37-generic/include/linux/mm_types.h
10. FILL:after_alloc_page()→_refcount=___→WHY:set_page_refcounted(page)_sets_it_to_1
11. FILL:after_get_page()→_refcount=___→WHY:atomic_inc(&page->_refcount)→1+1=2
12. FILL:after_first_put_page()→_refcount=___→WHY:atomic_dec_and_test→2-1=1→test:1==0?→✗→page_NOT_freed
13. FILL:after_second_put_page()→_refcount=___→WHY:atomic_dec_and_test→1-1=0→test:0==0?→✓→__free_page()_called
14. VERIFY_LINE_10:dmesg|grep_"after_get"→ref=2?→✓_or_✗
15. VERIFY_LINE_12:dmesg|grep_"after_put1"→ref=1?→✓_or_✗
16. VERIFY_LINE_13:dmesg|grep_"after_put2"→"page_freed"?→✓_or_✗

## FAILURE PREDICTIONS
17. F1:If_get_page_called_twice→_refcount=3→need_three_put_page_to_free→FILL:what_happens_if_only_two_put_page?
18. F2:If_put_page_called_on_freed_page→_refcount=0→put_page→0-1=-1→VM_BUG_ON→FILL:what_message_in_dmesg?
19. F3:If_alloc_page_returns_NULL→FILL:what_is_errno?→FILL:why_would_it_fail?

## CALCULATIONS (USER DOES BY HAND)
20. CALC:page=0xffffea0007a21f40→vmemmap=0xffffea0000000000→diff=0xffffea0007a21f40-0xffffea0000000000=___
21. CALC:diff_from_20=___→sizeof_page=64→pfn=diff/64=___
22. CALC:pfn_from_21=___→phys_addr=pfn*4096=___
23. CALC:phys_addr_from_22=___→zone=???→DERIVE:if_pfn<4096→DMA,elif_pfn<1048576→DMA32,else→Normal

## VIOLATION CHECK
24. NEW_THINGS_INTRODUCED_WITHOUT_DERIVATION:___
25. IF_LINE_24_NOT_EMPTY→WORKSHEET_REJECTED

## COMMANDS TO RUN
26. make
27. sudo insmod refcount_hw.ko
28. sudo dmesg | tail -20
29. sudo rmmod refcount_hw
30. gcc refcount_user.c -o refcount_user && ./refcount_user
