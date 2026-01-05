```
═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
KERNEL MM EXERCISES WORKSHEET - AXIOMATIC DERIVATIONS
═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Kernel: 6.14.0-37-generic | RAM: 15776276 kB | PAGE_SIZE: 4096 | nr_free_pages: 324735 | vmemmap: 0xFFFFEA0000000000 | PAGE_OFFSET: 0xFFFF888000000000
═══════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════════

DEFINITIONS (MUST KNOW BEFORE PROCEED):
D01. RAM=bytes on motherboard at address 0,1,2,...,15776276×1024-1 → WHY: hardware gives bytes
D02. PAGE=4096 contiguous bytes → WHY: managing 16GB byte-by-byte=16×10^9 entries=slow → 16GB/4096=4M entries=fast
D03. PFN=page frame number=physical_address÷4096 → WHY: kernel allocates pages not bytes → PFN=index into page array
D04. ZONE=label by address range → WHY: old hardware reads only low RAM → DMA<16MB, DMA32<4GB, Normal=rest
D05. struct page=64 bytes metadata per page → WHY: kernel tracks usage, flags, refcount per page
D06. vmemmap=0xFFFFEA0000000000=array of struct page → WHY: page_to_pfn(p)=(p-vmemmap)/64, pfn_to_page(n)=vmemmap+n×64
D07. _refcount=atomic counter at offset+52 in struct page → WHY: prevent free while in use → alloc→1, get→+1, put→-1, 0→free
D08. BUDDY=track free pages by power-of-2 sizes → WHY: finding N contiguous in bitmap=O(N), buddy find=O(log N)
D09. ORDER=power of 2 → order-N=2^N pages=2^N×4096 bytes → order0=4KB, order5=128KB, order10=4MB
D10. /proc/buddyinfo columns=order counts → column N=count of free 2^N-page blocks → total_pages=count×2^N

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
EXERCISE 1: PAGE_REFCOUNT_BUG - TRACE _refcount BY HAND
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

01. AXIOM: alloc_page(GFP_KERNEL) returns page pointer, sets _refcount=1 → SOURCE: mm/page_alloc.c:__alloc_pages → prep_new_page → set_page_count(page, 1)
02. AXIOM: get_page(page) does atomic_inc(&page->_refcount) → SOURCE: include/linux/mm.h:get_page → page_ref_inc
03. AXIOM: put_page(page) does atomic_dec_and_test(&page->_refcount), if result=0 then free → SOURCE: mm/swap.c:put_page → put_page_testzero
04. AXIOM: _refcount offset in struct page = 52 bytes (kernel version dependent, verify with pahole) → raw_address = page_ptr + 52
05. CALCULATE: If alloc_page returns page@0xFFFFEA0000100000, what is PFN? → pfn = (0xFFFFEA0000100000 - 0xFFFFEA0000000000) / 64 = 0x100000 / 64 = 0x4000 = 16384 → FILL: _____
06. CALCULATE: If pfn=16384, what is physical address? → phys = 16384 × 4096 = 67108864 = 0x4000000 = 64 MB → FILL: _____
07. CALCULATE: If pfn=16384, what zone? → 16384 ≥ 4096 ✓ (not DMA), 16384 < 1048576 ✓ (not Normal) → zone = DMA32 → FILL: _____
08. TRACE: alloc_page() → _refcount = ___ (FILL: 1)
09. TRACE: get_page() → _refcount = 1 + 1 = ___ (FILL: 2)
10. TRACE: put_page() → _refcount = 2 - 1 = ___ (FILL: 1), test if 0? → 1≠0 → ✗ free
11. TRACE: put_page() → _refcount = 1 - 1 = ___ (FILL: 0), test if 0? → 0=0 → ✓ FREE PAGE
12. TRACE: put_page() AGAIN → _refcount = 0 - 1 = ___ (FILL: -1 = 0xFFFFFFFF underflow), PAGE ALREADY FREE → BUG_ON triggered
13. PREDICT dmesg output after BUG: "BUG: Bad page state" or "page dumped because: nonzero _refcount" → kernel may panic
14. FIX: Comment out third put_page() call in page_refcount_bug.c → rebuild → reload → verify _refcount sequence 1→2→1→0→freed

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
EXERCISE 2: BUDDY_FRAGMENT - CALCULATE BUDDYINFO BY HAND
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

15. AXIOM: /proc/buddyinfo DMA32 = 24301 13358 5390 2767 773 444 242 61 35 23 23 (11 columns = order 0-10)
16. CALCULATE order0 bytes: 24301 × 2^0 × 4096 = 24301 × 1 × 4096 = 99,545,088 bytes = ___ MB (FILL: 94.9)
17. CALCULATE order1 bytes: 13358 × 2^1 × 4096 = 13358 × 2 × 4096 = 109,445,120 bytes = ___ MB (FILL: 104.3)
18. CALCULATE order2 bytes: 5390 × 2^2 × 4096 = 5390 × 4 × 4096 = 88,309,760 bytes = ___ MB (FILL: 84.2)
19. CALCULATE order5 bytes: 444 × 2^5 × 4096 = 444 × 32 × 4096 = 58,195,968 bytes = ___ MB (FILL: 55.5)
20. CALCULATE order10 bytes: 23 × 2^10 × 4096 = 23 × 1024 × 4096 = 96,468,992 bytes = ___ MB (FILL: 92.0)
21. CALCULATE total DMA32 pages: 24301×1 + 13358×2 + 5390×4 + 2767×8 + 773×16 + 444×32 + 242×64 + 61×128 + 35×256 + 23×512 + 23×1024 = ___ (FILL BY HAND)
22. ARITHMETIC: 24301 + 26716 + 21560 + 22136 + 12368 + 14208 + 15488 + 7808 + 8960 + 11776 + 23552 = 188873 pages × 4096 = 773,832,704 bytes = 737.9 MB
23. BUDDY XOR: If page at pfn=0x1000, order=0, buddy = 0x1000 XOR (1 << 0) = 0x1000 XOR 0x1 = 0x1001 → FILL: _____
24. BUDDY XOR: If page at pfn=0x1000, order=1, buddy = 0x1000 XOR (1 << 1) = 0x1000 XOR 0x2 = 0x1002 → FILL: _____
25. BUDDY XOR: If page at pfn=0x1000, order=2, buddy = 0x1000 XOR (1 << 2) = 0x1000 XOR 0x4 = 0x1004 → FILL: _____
26. FRAGMENTATION: Allocate 16 pages, free only odd-indexed → buddies not free → order0 +8, order1 unchanged → verify with /proc/buddyinfo

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
EXERCISE 3: GFP_CONTEXT_BUG - DECODE PREEMPT_COUNT BY HAND
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

27. AXIOM: preempt_count() bits: [0-7]=preempt depth, [8-15]=softirq mask, [16-19]=hardirq count, [20]=NMI
28. AXIOM: in_interrupt() = (preempt_count() & (HARDIRQ_MASK | SOFTIRQ_MASK | NMI_MASK)) != 0
29. AXIOM: in_atomic() = (preempt_count() != 0) typically, depends on config
30. AXIOM: spin_lock() calls preempt_disable() → preempt_count [0-7] += 1
31. AXIOM: GFP_KERNEL calls might_sleep() → checks in_atomic() → if true: WARNING "sleeping in atomic"
32. DECODE: preempt_count = 0x00000001 → bits[0-7]=1, bits[8-15]=0, bits[16-19]=0 → preempt disabled once, NOT in irq → in_atomic=true? ___ (FILL: depends on config)
33. DECODE: preempt_count = 0x00010000 → bits[16-19]=1 → in hardirq → in_interrupt=true, in_atomic=true
34. DECODE: preempt_count = 0x00000100 → bits[8-15]=1 → in softirq → in_interrupt=true, in_atomic=true
35. DECODE: preempt_count = 0x00100000 → bit[20]=1 → NMI → in_interrupt=true, in_atomic=true
36. CALCULATE GFP_KERNEL: __GFP_RECLAIM=0x400, __GFP_IO=0x40, __GFP_FS=0x80 → TOTAL = 0x400 | 0x40 | 0x80 = 0x4C0 (verify in your kernel)
37. FIX: Replace alloc_page(GFP_KERNEL) with alloc_page(GFP_ATOMIC) when under spinlock → GFP_ATOMIC=0 means no reclaim, no IO, no FS, no sleep

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
EXERCISE 4: STRUCT PAGE LAYOUT - CALCULATE OFFSETS BY HAND
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

38. AXIOM: sizeof(struct page) = 64 bytes (verify: pahole -C page /lib/modules/$(uname -r)/build/vmlinux or estimate)
39. AXIOM: struct page fields (approximate, varies by config): flags at +0 (8 bytes), _refcount at +52 (4 bytes), _mapcount at +48 (4 bytes)
40. CALCULATE page array size: MemTotal=15776276 kB → 15776276×1024 bytes → 15776276×1024/4096 pages = 3,850,411 pages
41. CALCULATE vmemmap size: 3,850,411 × 64 bytes = 246,426,304 bytes = 235 MB → ~1.5% RAM overhead for page tracking
42. CALCULATE page pointer: If pfn=1000000, page = vmemmap + 1000000 × 64 = 0xFFFFEA0000000000 + 64000000 = 0xFFFFEA0000000000 + 0x3D09000 = 0xFFFFEA0003D09000
43. CALCULATE physical: pfn=1000000 → phys = 1000000 × 4096 = 4,096,000,000 = 0xF4240000 → 3.8 GB → zone = DMA32 (< 4GB) or Normal (depends on exact boundary)
44. VERIFY: 1000000 < 1048576? → 1000000 < 1048576 ✓ → zone = DMA32

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
VERIFICATION COMMANDS
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

45. Build: cd exercises_mm && make 2>&1 | head -20
46. Load refcount: sudo insmod page_refcount_bug.ko && dmesg | tail -30 && sudo rmmod page_refcount_bug
47. Load buddy: cat /proc/buddyinfo && sudo insmod buddy_fragment.ko && dmesg | tail -40 && sudo rmmod buddy_fragment && cat /proc/buddyinfo
48. Load gfp: sudo insmod gfp_context_bug.ko && dmesg | tail -30 && sudo rmmod gfp_context_bug
49. Check free pages: cat /proc/vmstat | grep nr_free_pages

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
FAILURE PREDICTIONS
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

F01. User forgets _refcount starts at 1 → miscounts put_page calls → underflow
F02. User confuses count×2^order with count → buddyinfo interpretation wrong by factor of 2^N
F03. User uses GFP_KERNEL under spinlock → sleeping in atomic context warning
F04. User forgets order0 buddy XOR 1, order1 buddy XOR 2, order2 buddy XOR 4 → wrong buddy calculation
F05. User assumes struct page is exactly 64 bytes → varies by kernel config and version
F06. User forgets pfn×4096 for physical address → off by factor of 4096
F07. User confuses pfn (page index) with page pointer (vmemmap address) → type confusion
F08. User doesn't verify zone boundaries → assumes all pages from Normal → wrong
F09. User forgets GFP_ATOMIC can fail if no memory → NULL pointer dereference
F10. User assumes preempt_count=0 after spin_unlock → forgot nested locks

───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────
NEW THINGS INTRODUCED WITHOUT DERIVATION: (MUST BE EMPTY)
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────────

ALL terms in D01-D10. ALL axioms from /proc or kernel source. ALL calculations derived step-by-step.
If any term appears without definition → WORKSHEET REJECTED → trace back to axiom source.
```
