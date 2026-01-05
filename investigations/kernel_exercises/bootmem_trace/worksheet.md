# KERNEL MEMORY MANAGEMENT: AXIOMATIC DERIVATION WORKSHEET
## Bootmem Trace Exercise | Linux Kernel 6.8.0 | x86_64

---

### MACHINE SPECIFICATIONS
| Property | Value | Source Command |
|----------|-------|----------------|
| RAM | 15776276 kB = 15.04 GB | `cat /proc/meminfo \| grep MemTotal` |
| PAGE_SIZE | 4096 bytes | `getconf PAGE_SIZE` |
| Total Pages | 3944069 | 15776276×1024÷4096 |
| Kernel | 6.14.0-37-generic | `uname -r` |
| vmemmap | 0xffffea0000000000 | Kernel headers |

---

### TABLE OF CONTENTS
1. [Lines 01-20] **RAM → Pages → PFN**: Fundamental memory division
2. [Lines 21-36] **struct page → vmemmap**: Metadata tracking for each PFN
3. [Lines 37-50] **Zones**: DMA, DMA32, Normal memory regions
4. [Lines 51-70] **_refcount**: Reference counting mechanism
5. [Lines 71-100] **GFP Flags**: Allocation permission bitmask
6. [Lines 100-200] **Buddy Allocator**: Free list organization by order
7. [Lines 200-360] **Buddy Algorithm**: Split on alloc, merge on free, XOR trick
8. [Lines 360-521] **Source Code Proofs**: Kernel source verification

---

### LESSON OBJECTIVES
After completing this worksheet, you will be able to:
- [ ] Calculate PFN from physical address and vice versa
- [ ] Trace struct page location using vmemmap arithmetic
- [ ] Determine zone membership from PFN
- [ ] Explain refcount lifecycle: alloc→get→put→free
- [ ] Derive buddy PFN using XOR formula
- [ ] Verify all formulas against kernel source code

---

### NOTATION GUIDE
| Symbol | Meaning |
|--------|---------|
| ✓ | Verified/Correct |
| ✗ | Failed/Incorrect |
| → | Leads to / Implies |
| ∴ | Therefore |
| AXIOM | Accepted truth from source |
| DEFINITION | New term being defined |
| CALCULATION | Step-by-step arithmetic |
| DRAW | Visual diagram |
| PROOF | Source code reference |

---

## PART 1: RAM TO PAGES
---

01. AXIOM: Computer has RAM (Random Access Memory). RAM = hardware chip storing data.
02. AXIOM: RAM contains BYTES. 1 byte = 8 bits. 1 bit = 0 or 1.
03. AXIOM: Each byte has ADDRESS. Address = number identifying byte location. First byte = address 0. Second byte = address 1. N-th byte = address N-1.
04. AXIOM: This machine has 15776276 kB RAM. Source: cat /proc/meminfo | grep MemTotal → output: MemTotal: 15776276 kB.
05. DEFINITION: kB = kilobyte. 1 kB = 1024 bytes. WHY 1024? 1024 = 2^10. Binary computers use powers of 2.
06. CALCULATION: 2^10 = 2×2×2×2×2×2×2×2×2×2 = 1024. Verify: 2×2=4 → 4×2=8 → 8×2=16 → 16×2=32 → 32×2=64 → 64×2=128 → 128×2=256 → 256×2=512 → 512×2=1024. ✓
07. CALCULATION: Total bytes = 15776276 kB × 1024 bytes/kB. Calculate by hand: 15776276 × 1024.
08. CALCULATION: 15776276 × 1024 = 15776276 × 1000 + 15776276 × 24 = 15776276000 + 378630624 = 16154906624 bytes. This machine has 16154906624 bytes of RAM.
09. PROBLEM: Kernel must track each byte. 16154906624 bytes = 16154906624 tracking entries. Too many. Too slow.
10. SOLUTION: Group bytes into PAGES. Track pages, not bytes.
11. DEFINITION: PAGE = fixed-size block of contiguous bytes. Contiguous = addresses are consecutive (N, N+1, N+2, ...).
12. AXIOM: On this machine, PAGE size = 4096 bytes. Source: getconf PAGE_SIZE → output: 4096.
13. CALCULATION: 4096 = 2^12. Verify: 2^10 = 1024 (line 06) → 2^11 = 1024×2 = 2048 → 2^12 = 2048×2 = 4096. ✓
14. DEFINITION: PFN = Page Frame Number. PFN = index of page. First page = PFN 0. Second page = PFN 1. N-th page = PFN N-1.
15. FORMULA: PFN = byte_address ÷ 4096. WHY? Each page has 4096 bytes. Page 0 = bytes 0-4095. Page 1 = bytes 4096-8191. Page N = bytes N×4096 to (N+1)×4096-1.
16. EXAMPLE: Byte address 8000. PFN = 8000 ÷ 4096 = 1 (integer division). Verify: Page 1 = bytes 4096-8191. 8000 is in range 4096-8191. ✓
17. FORMULA: Physical address = PFN × 4096. WHY? First byte of page N is at address N × 4096.
17a. DEFINITION: phys_addr_t = kernel type for physical addresses. Source: /usr/src/linux-source-6.8.0/include/linux/types.h.
17b. CODE: #ifdef CONFIG_PHYS_ADDR_T_64BIT → typedef u64 phys_addr_t; #else → typedef u32 phys_addr_t;
17c. THIS MACHINE: x86_64 → CONFIG_PHYS_ADDR_T_64BIT=y → phys_addr_t = u64 = unsigned long long = 8 bytes = 64 bits.
17d. INCLUDED VIA: #include <linux/mm.h> → includes <linux/types.h> → phys_addr_t available.
18. EXAMPLE: PFN 5. Physical address = 5 × 4096 = 20480. Verify: Page 5 = bytes 20480-24575. First byte = 20480. ✓
19. CALCULATION: Total pages = 16154906624 bytes ÷ 4096 bytes/page. Calculate by hand: 16154906624 ÷ 4096.
20. CALCULATION: 16154906624 ÷ 4096 = 16154906624 ÷ 4 ÷ 1024 = 4038726656 ÷ 1024 = 3944069 pages. This machine has 3944069 pages.

---

## PART 2: STRUCT PAGE AND VMEMMAP
---

21. PROBLEM: Kernel needs metadata for each page. Metadata = information about page (is it free? who is using it? how many users?).
22. SOLUTION: Create data structure for each PFN -- NOT PAGE -- THIS WAS WRONG TO BE INTRODUCED -- BECAUSE YOU ARE TALKING ABOUT PFN AND ALL OF A SUDDEN YOU JUMP TO PAGE WHICH IS BAD FOR WORKING MEMORY OF BRAIN . Data structure = collection of related variables.
23. DEFINITION: struct page = kernel data structure. One struct page exists for each physical page. struct page contains: flags, _refcount, _mapcount, other fields. WHAT IS COLLECTION TO PFN DOES IT CONTAIN ?
24. AXIOM: sizeof(struct page) = 64 bytes. Source: pahole -C "struct page" vmlinux OR kernel source code.  - DID NOT TELL WHATT THIS COMMAND DOEWS AND WHY
25. VERIFY: 64 = 2^6. 2^6 = 2×2×2×2×2×2 = 64. ✓
26. CALCULATION: Memory needed for all struct pageS = 3944069 pages fRAME NUMBERS × 64 bytes/page = 
- IT IS WRONG TO INTROUCE THIS -- BECAUSE WE WERE TALKING ABOUT PAGE FRAME NUMBERS AS IDENTIFIERS TO A CHUNK OF PHYSICAL MEMORY 
27. CALCULATION: 3944069 × 64 = 3944069 × 60 + 3944069 × 4 = 236644140 + 15776276 = 252420416 bytes.TO TRACK EACH OF THE PAGE FRAME NUMBERS WE ARE NOW ADDING DATA - COULD WE NOT JUST USE INTS TO TRACK THE PAGE FRAME NUMBERS 
28. DEFINITION: MB = megabyte. 1 MB = 1024 kB = 1024 × 1024 bytes = 1048576 bytes.
29. CALCULATION: 1048576 = 2^20. Verify: 2^10 = 1024 (line 06) → 2^20 = 2^10 × 2^10 = 1024 × 1024 = 1048576. ✓
30. CALCULATION: 252420416 bytes ÷ 1048576 bytes/MB = 240.7 MB. Kernel uses 240.7 MB to track all pages on this machine.
31. DEFINITION: vmemmap = array of struct page. Array = contiguous sequence of same-type elements. vmemmap[0] = struct page for PFN 0. vmemmap[N] = struct page for PFN N.-- BUT WHY CALL IT A PAGE -- JUST A STRUCT ID FOR EACH OF THE PAGE FRAME NUMBERS 
32. AXIOM: vmemmap base address = 0xffffea0000000000. Source: grep VMEMMAP_START /usr/src/linux-headers-$(uname -r)/arch/x86/include/asm/pgtable_64_types.h.-- IS THIS HARDCODED IN ALL MACHINES ? 
33. FORMULA: pfn_to_page(pfn) = vmemmap + (pfn × 64). Returns pointer to struct page for given PFN.- WHY I WANT THIS -- PFN IS AN INT THEN I WANT A STRUCTT - WHY WOULD I NEED THIS -- LET ME THINK -- BASE ADDRESS OF STRUCT WHERE THE IDENTIFIER OF FIRST PAGE FRAME IS STORED IN DRAM -- OK 
-- SO THE BASE ADDRESS OF THE STRUCT WRAPPER WHICH IDS THE FIRST PAGE FRAME -- BUT HOW DO WE KNOW THAT THIS IS THE ID OF THE FIRST PFN -- BECAUSE WE KNOW THE BASE ADDRESS -- OLDEST TRICK -- USE A LOCATION AND ASSUME THAT IT CONTAINS THE ADDDRESS OF A HOMOGENOUS TYPE AND THEN KEEP INCREASING POINTER LENGTH TO JUMP -- SO PFN TO PAGE MEANS -- takes an INT -- AND THEN ASSUMES THAT THE BASE ADDRESS ALSO CONTAINS THE SAME TYPE AND THEN "GOES" TO T HE BASE ADDRESS ADD ITSLEF AND JUMPS THE POINTER BY SIZE OF EACH WRAPPER - THINK OF THE STRUCT AS A JAIL IN WHICH PFN LIVES 
34. FORMULA: page_to_pfn(page) = (page - vmemmap) ÷ 64. Returns PFN for given struct page pointer. -- now what is this -- PAGE IS THE JAIL AND JAIL WANTS TO KNOW THE ID -- DOES IT NOT HAVE THE KEY OF EACH JAIL AND LOCK OF THE JAIL AND THE KEY -- OF THE LOCK OF THE JAIL THIS IS HORRIBLE TO READ --- LET ME THINK THE JAIL DOES NOT KNOW THE ID OF THE INNMATE IT IS PROTECTING LOL -- I MEAN JAILING -- SO IT GOES TO WHERE -- BASE ADDRESS OF FIRST JAIL IN THE JAIL BUILDING -- SEES HOW FAR IT IS FROM THAT JAIL THE FIRSTT JAIL AND THEN DOES WHAT?? DOES WHAT?? THIS IS SUFFICIENT TO KNOW THE JAIL INDEX OF THAT JAIL -- IS IT??? NO IT WILL TELL THE DIFFERENCE IN ADDRESS -- IF I HUBLE THIS DOWN BY 64 THEN I KNOW WHICH JAIL I AM -- BUT THAT TELLS ME THE ID OF THE INNMATE I AM CARRYING -- SO THE ID OF THE INNMATE I AM CARRYING IS THE SAME PLACE I AM IN THE JAIL BUILDING -- MAY I SHOULD CALL MYSELF A CELL NOT A JAIL 


35. EXAMPLE: PFN = 1000. page = 0xffffea0000000000 + (1000 × 64) = 0xffffea0000000000 + 64000 = 0xffffea000000fa00.
1000 IS THE ID OF THE INNMATE IN MY CELL -- THE BASE ADDRESS OF THE FIRST CELL IN THE JAIL BUILDING IS THAT FFFFFF THINGS -- ESCLATE THIS UP -- TO GET THE JAIL ADDRESS I MEAN CELL ADDRESS 

36. VERIFY: 64000 in hex. 64000 ÷ 16 = 4000 r0 → 4000 ÷ 16 = 250 r0 → 250 ÷ 16 = 15 r10=A → 15 ÷ 16 = 0 r15=F → read bottom-up: FA00. 64000 = 0xFA00. ✓

---

## PART 3: MEMORY ZONES
---

37. PROBLEM: Old hardware cannot access all RAM addresses. 24-bit address bus = can only address 2^24 = 16777216 bytes = 16 MB.
38. PROBLEM: 32-bit hardware cannot access addresses above 2^32 = 4294967296 bytes = 4 GB.
39. SOLUTION: Divide RAM into ZONES. Zone = region of RAM with specific address range.
    OK SO WHAT -- RAM INTO ZONES - THEN WHAT DO I CARE - AND WHY -- ZONES ARE WHAT -- AND WHY DO I CARE -- RAM IS DIVIDED I THOUGHT WE WERE DIVIFING INTO CELLS AND JAILING THEM USIGN PFN AS ID NOW I HAZE ZONES
40. DEFINITION: ZONE_DMA = pages with addresses < 16 MB. For old 24-bit DMA hardware.
-- AGAIN THIS IS BAD -- WE NEVER DEFINED PAGES WE DEFINED PFN ONLY 
-- PFN WITH ADDRESS <16 mb -- THAT IS ODD 
41. DEFINITION: ZONE_DMA32 = pages with addresses 16 MB to 4 GB. For 32-bit hardware.
  -- THIS IS ODD AS WELL PFN WITH ADDRESS FROM 16 TO 4GB -- PFN ARE JUST NUMBERS THEY ID THE RAM BLOCKS WITHIN CHUNKS 
42. DEFINITION: ZONE_NORMAL = pages with addresses ≥ 4 GB. For modern 64-bit hardware.
43. CALCULATION: DMA zone boundary PFN = 16 MB ÷ 4096 bytes/page.
   --- THIS IS MEANING THAT -- NOW THIS IS GOOD - WE ARE BACK TO PFN 
   --- PFNS ALL PFN WHICH ADDRESS MORE THAN 4GB 
   -- SO ALL PFN WITH NORMAL 4GB RAM IS ABNORMAL 
   -- THIS IS ODD BECAUSE ON MY MACHINE 
   -- BECAUSE I HAVE 16GB OF RAM 
   -- ON MY MACHINE I HAVE PFN AND ADDRESSING ISSUES -- THIS IS  BAD -- WHY DO I CARE ABOUT THIS ADDRESSING AT THIS STAGE -- IS IT THE BUS?
44. CALCULATION: 16 MB = 16 × 1048576 bytes (line 28) = 16777216 bytes.
45. CALCULATION: 16777216 ÷ 4096 = 4096. DMA zone ends at PFN 4096.
-- LET ME SEE -- 16MB IS SO MANY BYTES AND I NEED HOW MANY PFNS -- SINCE ONE PFN IS COVERING FOR 
-- 4096 BYTES I HAVE USED HOW MANY -- /4096
46. RULE: PFN < 4096 → zone = DMA.
47. CALCULATION: DMA32 zone boundary PFN = 4 GB ÷ 4096 bytes/page.
    -- NOW LET ME SEE THIS -- THIS IS BAD PFN -- BAD FOR MY BRAIN 
    -- how many more pfn I NEED - SO BAD ON MY HEAD 
    -- SO MANY PFN -- 4GB IS SO MANY BYTES 
    1024 * 1024 → 1048576 ✓
    1024 * 1048576 → 1073741824 ✓
    4 * 1073741824 → 4294967296 ✓
    4294967296 / 4096 → 1048576 ✓
    ∴ 4 GB / 4096 → 1048576 ✓
48. CALCULATION: 4 GB = 4 × 1024 MB × 1048576 bytes/MB = 4 × 1024 × 1048576 = 4294967296 bytes.
49. CALCULATION: 4294967296 ÷ 4096 = 1048576. DMA32 zone ends at PFN 1048576.
50. RULE: 4096 ≤ PFN < 1048576 → zone = DMA32.
51. RULE: PFN ≥ 1048576 → zone = Normal.

---

## PART 4: REFERENCE COUNTING (_refcount)
---

52. DEFINITION: _refcount = reference count. Integer tracking how many users are using a page. WHY? Prevent freeing page while someone still using it.
who are these users and why do i care/ i thought at time of boot there is 1:1 mapping and no user space process can ever watch any other process virtual memory 
53. BEHAVIOR: alloc_page() sets _refcount = 1. Page has 1 user (the allocator).
-- hence each page will have one ref count at start and what happens whenuser space code does a malloc - trace the entire path from malloc to this alloc page - let us say there are 100s of process doing malloc
54. BEHAVIOR: get_page(page) increments _refcount. _refcount = _refcount + 1. New user acquired page.
- why get increases i thought the api means get to get the studd and it isconst only for get functions 
55. BEHAVIOR: put_page(page) decrements _refcount. _refcount = _refcount - 1. User released page.
- this is odd because put means to put  a value inapis and it is non const but in this case it is reducing values
56. BEHAVIOR: If _refcount reaches 0 → page is freed. Returned to buddy allocator.
-- you introduced buddy without defining it -- and who does it when free happens does this happen 
-- what does it mean retutned -- we just said count is zero hence it should beinvalud
57. BUG: If put_page called when _refcount already 0 → _refcount = 0 - 1 = -1 → INVALID → kernel panics or warns.
-- really? are there no checks or wrappers to prevent this -what if user space code keeps calling free all the time 

---

## PART 5: GFP FLAGS (Get Free Page)
---

58. DEFINITION: GFP = Get Free Page. GFP flags tell kernel how to allocate memory.
-- what do you mean how -- we are talking about page frame numbers being placeholders for ram -- what is need of all this -- get from where -- from ram but i already purchased ram 
59. DEFINITION: GFP_KERNEL = flags for normal kernel allocation. Process can sleep. Can do disk I/O. Can use filesystem. -- no whatt is normal-- who can sleep because at boot t ime all entries are there in the pml4 page so whatt is need of this flag at all
60. AXIOM: GFP_KERNEL = 0xCC0. Source: grep GFP_KERNEL /usr/src/linux-headers-$(uname -r)/include/linux/gfp.h. -- what is cc0 it is 12 1011011
61. DERIVATION: GFP_KERNEL = __GFP_RECLAIM | __GFP_IO | __GFP_FS. __GFP_RECLAIM = ___GFP_DIRECT_RECLAIM | ___GFP_KSWAPD_RECLAIM. --what is this or we arre talking about and why we do or so many times teach using nums ::Explain using ONLY numbers, symbols, and arrows. Zero English words. Show input → computation → output. Each line: one fact or one calculation. Use ✓ for true, ✗ for false. Use ∴ for therefore. Use → for implies or leads to
62. AXIOM FROM KERNEL SOURCE (line 27-38 of gfp_types.h): Bit positions are enum values. ___GFP_IO_BIT = 6 (7th enum, counting from 0 at ___GFP_DMA_BIT). ___GFP_FS_BIT = 7. ___GFP_DIRECT_RECLAIM_BIT = 10. ___GFP_KSWAPD_RECLAIM_BIT = 11.
-- but why need these types at all is pfn not enough at all?

63. CALCULATION: BIT(n) = 1 << n = 2^n. ___GFP_IO = BIT(6) = 2^6 = 64 = 0x40. ___GFP_FS = BIT(7) = 2^7 = 128 = 0x80. ___GFP_DIRECT_RECLAIM = BIT(10) = 2^10 = 1024 = 0x400. ___GFP_KSWAPD_RECLAIM = BIT(11) = 2^11 = 2048 = 0x800. this is bad on my brain because i do not know anything at all about all this why would i want to do this -- what is reclaim does the ram reclaim it i thought user space process calls free and kernel calls kfree but who reclaims from whom and why 

64. CALCULATION: __GFP_RECLAIM = 0x400 | 0x800 = 1024 + 2048 = 3072 = 0xC00. == what doe these mean who set them up and why explain from scratch 
65. CALCULATION: GFP_KERNEL = 0xC00 | 0x40 | 0x80 = 3072 + 64 + 128 = 3264 = 0xCC0. ✓ - why do have this ata ll what is gfp kernel ata ll 
66. VERIFY: 0xCC0 in binary. 0xC = 1100. 0xCC0 = 1100 1100 0000. Bit 6 set (0x40). Bit 7 set (0x80). Bit 10 set (0x400). Bit 11 set (0x800). ✓ tjos os bad on my head 
67. AXIOM VERIFIED: GFP_KERNEL = 0xCC0. Source: kernel enumeration + BIT() macro calculation. - how does it relate to t he work done at boot time by the kernel in mapping pages and what is this gfp kernel derivation at all 

68. DEFINITION: alloc_page(gfp_mask) = kernel function. Allocates 1 page. Returns pointer to struct page. Sets _refcount = 1.== allocates from where it was already done at boot time then why not use the pfn 
69. DEFINITION: page_to_pfn(page) = kernel macro. Returns PFN for given struct page pointer. Uses formula line 34.= so i want the struct from the pfn -- that is easy alli need to add the base address of start of the addrerss of thr struct attay which is rapper around the pfn s 
70. DEFINITION: page_ref_count(page) = kernel function. Returns current _refcount value.
== - this takes a struct that is bad on my head -- what if two peopole call it togehter 
-- is this a const function 
71. DEFINITION: get_page(page) = kernel function. Increments _refcount by 1.-- but alloc page already did that 
72. DEFINITION: put_page(page) = kernel function. Decrements _refcount by 1. If reaches 0, frees page.- what if i call it on ileegal page 
73. NOW READY: Open bootmem_trace.c. All terms defined above. Code uses only defined terms.
74. CODE LINE 37: page_ptr = alloc_page(GFP_KERNEL); → Uses alloc_page (line 68), GFP_KERNEL (line 59-67). Returns struct page* (line 23).
75. CODE LINE 47: if (!page_ptr) → Check if alloc_page returned NULL. NULL = allocation failed.
76. CODE LINE 61: pfn = page_to_pfn(page_ptr); → Uses page_to_pfn (line 69). Returns PFN (line 14).-- why would i do this i wantt the struct and then why do i care with the struct i mean these structt were there at the boot time when kernel set up pages i am confused at this stage 
77. CODE LINE 75: phys = (phys_addr_t)pfn * PAGE_SIZE; → Uses formula line 17. PAGE_SIZE = 4096 (line 12). -- waht is this typecast i thougt ram address cannott be deref by kernel and need va macro then who and why can i do this typecase and why page size in capital this seems pfn was an int at start which was wrapping up pages size from the ram and then why i need typecase and this will give a a lerge address if the pfn is the base address but it is not a base address it is any random pfg this will give me what -- let us say pfn is 123 then why would i * by page size -- what do i get -- i get the ram address but i cannot do anythign with the ram address - how does it relate to the cre3 cr3 register at all 
78. CODE LINE 85: ref = page_ref_count(page_ptr); → Uses page_ref_count (line 70). Expected value = 1 (line 53). -- what is tyope fi ref and why do i care - what is page_ptr type and why do i need it 
79. CODE LINE 97: Zone determination using pfn >= 1048576 → Uses rules lines 46, 50, 51. 1048576 from line 49. - let us say pfn is large and coveres a large ram but whatt about the boot time struct and what did the mapping ofbelow 512 entries for each process do with the pf being in this range -- i mean pfn is let us say a user space address is someting how does it matter what range the pfg will like -- i mean i have already done the indexing and indexing in pml4 3 and pd to reach here -- 
80. CODE LINE 107: get_page(page_ptr); → Uses get_page (line 71). _refcount: 1 → 2 (line 54).
81. CODE LINE 117: put_page(page_ptr); → Uses put_page (line 72). _refcount: 2 → 1 (line 55).
82. CODE LINE 130: put_page(page_ptr); → _refcount: 1 → 0 (line 55). Page freed (line 56).
83. CODE LINE 143: put_page(page_ptr); → BUG (line 57). _refcount: 0 → -1.
84. EXECUTE: cd /home/r/Desktop/learnlinux/kernel_exercises/bootmem_trace.
85. EXECUTE: make → Compiles bootmem_trace.c → Generates bootmem_trace.ko.
86. EXECUTE: cat /proc/buddyinfo → BEFORE insmod → Record Normal zone order-0 count.
87. EXECUTE: sudo insmod bootmem_trace.ko → Loads module.
88. EXECUTE: dmesg | tail -30 → Read output. Find page_ptr, pfn, phys, ref values.
89. COMPARE: Observed pfn with calculated pfn (if you predicted one). Recalculate if different.
90. EXECUTE: sudo rmmod bootmem_trace → Unloads module.
91. NEW THINGS INTRODUCED WITHOUT DERIVATION: None. All terms defined before use.
92. ---
93. ERROR REPORT: SESSION 2025-12-27
94. ---
95. E01. Line: Initial worksheet version. Wrong: Used "kB" without defining kB. Should: Define kB = 1024 bytes before using. Sloppy: Assumed reader knows kB. Missed: kB is not universal axiom. Prevent: Define every unit before use.
96. E02. Line: Initial worksheet version. Wrong: Used "PFN" without definition. Should: Define PFN = Page Frame Number = page index. Sloppy: Jumped to calculation. Missed: PFN is kernel-specific term. Prevent: No acronym without expansion + definition.
97. E03. Line: Initial worksheet version. Wrong: Used "vmemmap" without explaining what it is. Should: Define vmemmap = array of struct page. Sloppy: Assumed reader understands kernel internals. Missed: vmemmap is implementation detail. Prevent: Build concepts bottom-up.
98. E04. Line: Initial worksheet version. Wrong: Used "zone" without problem statement. Should: State problem first (old hardware can't access all RAM), then solution (zones). Sloppy: Listed zones without WHY. Missed: WHY is more important than WHAT. Prevent: Every concept needs motivation.
99. E05. Line: Initial worksheet version. Wrong: Claimed GFP_KERNEL=0xCC0 as axiom. Should: Derive from bit positions. Sloppy: Copy-pasted value. Missed: 0xCC0 is not obvious. Prevent: Show calculation for every magic number.
100. E06. Line: GFP calculation. Wrong: First attempt got 0x4C0 instead of 0xCC0. Should: Check kernel source for __GFP_RECLAIM composition. Sloppy: Assumed __GFP_RECLAIM = 0x400. Missed: __GFP_RECLAIM = ___GFP_DIRECT_RECLAIM | ___GFP_KSWAPD_RECLAIM = 0x400 | 0x800 = 0xC00. Prevent: Trace every macro to its base definition.
101. E07. Line: Zone boundaries. Wrong: Said "16 MB" and "4 GB" without source. Should: Cite ISA spec for 24-bit DMA limit, PCI spec for 32-bit limit. Sloppy: Treated architecture constants as obvious. Missed: These are hardware constraints, not arbitrary numbers. Prevent: Every constant needs source.
102. E08. Line: PFN=2001021. Wrong: Invented number without runtime verification. Should: Say "PFN unknown until runtime, read from dmesg". Sloppy: Wanted example to calculate. Missed: Example should be marked as PREDICTION, not FACT. Prevent: Distinguish axiom vs prediction.
103. E09. Line: struct page size=64. Wrong: Stated without verification command. Should: Show pahole command or offsetof() calculations. Sloppy: Memorized value. Missed: struct page layout varies by kernel config. Prevent: Every struct size needs runtime verification.
104. E10. Line: _refcount behavior. Wrong: Claimed "alloc_page sets _refcount=1" without code path. Should: Trace alloc_page → __alloc_pages → prep_new_page → set_page_refcounted → atomic_set. Sloppy: Stated behavior without proof. Missed: Behavior is defined by code, not documentation. Prevent: Every behavior claim needs code trace.
105. E11. Line: Code comments. Wrong: Initial version had TODO blocks with vague instructions. Should: Each comment must show exact calculation with real numbers. Sloppy: Treated comments as roadmap instead of derivation. Missed: Comments are for CALCULATION, not DESCRIPTION. Prevent: Comment must answer: what number, how calculated, from what source.
106. E12. Line: Response format. Wrong: Used headings, sections, bullet points. Should: Dense lines, no breaks, no formatting decorations. Sloppy: Followed standard markdown conventions. Missed: User specified dense format. Prevent: Read user rules before generating.
107. E13. Line: Definition order. Wrong: Referenced "buddy allocator" before defining it. Should: Define buddy allocator before referencing. Sloppy: Assumed sequential reading. Missed: Each line must be self-contained. Prevent: Forward references forbidden.
108. E14. Line: 2^20 calculation. Wrong: Said "1 MB = 1048576" without deriving 2^20. Should: Show 2^10 × 2^10 = 1024 × 1024 = 1048576. Sloppy: Skipped intermediate step. Missed: User cannot verify skipped steps. Prevent: Every power of 2 needs explicit multiplication.
109. E15. Line: Hex conversion. Wrong: Some conversions lacked step-by-step division. Should: Show every division: N÷16=Q rR → digit=R. Sloppy: Used calculator mentally. Missed: Hand calculation is the exercise. Prevent: Write every division step.
110. ---
111. ORTHOGONAL ANALYSIS: WHY BRAIN IS SLOPPY
112. ---
113. O01. Pattern: Skip axiom → jump to calculation. Root: Decades of reading solutions, not deriving them. Brain trained to memorize answers.
114. O02. Pattern: Use term before defining. Root: Expert blindness. Term is "obvious" after repetition. Forgot learning process.
115. O03. Pattern: Claim value without source. Root: Trust memory over verification. Memory fails. Verification cannot fail.
116. O04. Pattern: Provide answer before question. Root: Impatience. Want to show result, not process. Process IS the learning.
117. O05. Pattern: Use headings and formatting. Root: Aesthetic habit. Formatting hides lack of content. Dense text forces density of thought.
118. O06. Pattern: Forward reference. Root: Outline thinking. Plan document structure first. Execute structure second. Structure before content = content suffers.
119. O07. Pattern: Magic numbers. Root: Cargo cult. Copy value from Stack Overflow. Never trace to source. Source is the only truth.
120. O08. Pattern: Assume "obvious". Root: Nothing is obvious. Every fact was learned. Every learning can be forgotten. Every assumption can be wrong.
121. ---
122. PREVENTION RULES (HARDCODE INTO BRAIN)
123. ---
124. P01. Before using ANY term: Is it defined above? No → Define first.
125. P02. Before using ANY number: Is it calculated above? No → Calculate first.
126. P03. Before claiming ANY behavior: Is code path traced? No → Trace first.
127. P04. Before writing ANY line: Does it use only terms from previous lines? No → Reorder.
128. P05. Before formatting: Is formatting necessary? No → Remove.
129. P06. Before finishing: Can someone verify every claim by running a command? No → Add command.
130. P07. After finishing: List all new things introduced. List should be empty. Non-empty = rejected.
131. ---
132. QUESTION SLOPPY BRAIN
133. ---
134. Q01. You skipped the definition. Did you save 5 seconds? Reader lost 5 minutes. Net loss.
135. Q02. You used a magic number. How will you debug when it's wrong? You won't remember source.
136. Q03. You jumped ahead. Who benefits? Your impatience. Who suffers? Reader's understanding.
137. Q04. You added heading. What content is under heading? If little, heading is filler. Remove.
138. Q05. You wrote "obvious". To whom? Not to beginner. Beginner is target. You are not target.
139. Q06. You memorized this. For how long? Until next distraction. Derivation lasts forever.
140. Q07. You trusted yourself. Track record? Errors above prove track record is bad. Trust verification.
141. Q08. You wanted to finish fast. Finish what? Broken document? Broken is not finished.
142. ---
143. END ERROR REPORT
144. ---
145. ANSWERS TO YOUR QUESTIONS (FROM SCRATCH)
146. ---
147. Q(line22): WHY JUMP FROM PFN TO PAGE? ANSWER: PFN=integer=4bytes. Problem: kernel needs MORE data per PFN: is_free?=1bit, who_owns?=pointer=8bytes, refcount=4bytes, flags=8bytes. Solution: wrap PFN with more data → struct page. DRAW: [PFN=1000]→wrap→[struct page: flags(8bytes)|_refcount(4bytes)|_mapcount(4bytes)|lru(16bytes)|...|total=64bytes]. PFN is still there, just wrapped.
148. Q(line23): WHAT IS CONNECTION PFN TO STRUCT PAGE? ANSWER: PFN 1000 → struct page at vmemmap[1000] → vmemmap[1000] contains metadata FOR that PFN → DRAW: vmemmap[0]=metadata_for_PFN_0, vmemmap[1]=metadata_for_PFN_1, vmemmap[1000]=metadata_for_PFN_1000. struct page does NOT contain PFN as field. PFN = array index.
149. Q(line24): WHAT IS PAHOLE? ANSWER: pahole = tool that reads debug info from ELF binary → shows struct layout. pahole -C "struct page" vmlinux → reads /usr/lib/debug/boot/vmlinux → outputs: struct page { unsigned long flags; /* 0 8 */ atomic_t _refcount; /* 8 4 */ ... /* size: 64 */ }. CALCULATE: offset+size → flags@0+8bytes, _refcount@8+4bytes.
150. Q(line27): WHY NOT JUST USE INT FOR PFN? ANSWER: CALCULATE what kernel needs per page: need_to_track=is_free(1bit)+refcount(32bits)+flags(64bits)+lru_pointers(128bits)+mapping(64bits)=289bits≈40bytes_minimum. int=4bytes=32bits. 32bits<289bits. CANNOT FIT. ∴ struct needed.
151. Q(line31): WHY CALL IT "PAGE" NOT "PFN_METADATA"? ANSWER: Historical naming. struct page = metadata for one physical page frame. Better name: struct pfn_descriptor or struct frame_metadata. Kernel uses "page" = confusing. YOUR MENTAL MODEL: struct page = METADATA_WRAPPER for PFN. struct page IS NOT the 4096 bytes of RAM.
152. Q(line32): IS VMEMMAP HARDCODED? ANSWER: grep VMEMMAP_START /usr/src/linux-headers-$(uname -r)/arch/x86/include/asm/pgtable_64_types.h → shows: #define __VMEMMAP_BASE_L4 0xffffea0000000000UL. This is COMPILE-TIME constant for x86_64 with 4-level page tables. Different architectures use different addresses. ARM64 uses 0xfffffffe00000000. KASLR may randomize at runtime.
153. Q(line33-34): YOUR JAIL ANALOGY IS CORRECT. DRAW: JAIL_BUILDING=vmemmap=0xffffea0000000000. CELL[0]=0xffffea0000000000. CELL[1]=0xffffea0000000040(+64). CELL[1000]=0xffffea000000fa00(+64000). INMATE_ID=PFN. CELL knows its index by: (CELL_address - JAIL_BUILDING_address) / CELL_size = (0xffffea000000fa00 - 0xffffea0000000000) / 64 = 0xfa00 / 64 = 64000 / 64 = 1000 = INMATE_ID.
154. Q(line39-46): WHY ZONES? PROBLEM: Old ISA device (24-bit address bus) → max_address=2^24-1=16777215. DRAW: bus_lines=[A0..A23]=24_wires→max_value=2^24=16777216. If RAM_address > 16777216 → device CANNOT send address → DMA fails. SOLUTION: Reserve RAM_below_16MB for old devices → call it ZONE_DMA. CALCULATE: 16MB/4096=4096 → PFN<4096 → ZONE_DMA. Your 16GB RAM: PFN 0-4095=DMA, PFN 4096-1048575=DMA32, PFN 1048576-3944069=Normal.
155. Q(line52): WHO ARE USERS OF _REFCOUNT? ANSWER: USER_1: kernel allocates page for process heap → refcount=1. USER_2: kernel maps SAME page to another process (shared memory) → get_page() → refcount=2. USER_3: kernel uses page for page cache (file backed) → refcount=3. DRAW: [struct page]→_refcount=3→[process_A:mmap][process_B:mmap][page_cache]. When each user finishes → put_page() → refcount:3→2→1→0→freed.
156. Q(line53): TRACE MALLOC TO ALLOC_PAGE. DRAW: user_calls_malloc(4096)→libc_malloc→brk()/mmap()syscall→kernel:do_brk_flags()/do_mmap()→vm_area_alloc()→page_fault_later→handle_mm_fault()→do_anonymous_page()→alloc_page(GFP_HIGHUSER_MOVABLE)→__alloc_pages()→get_page_from_freelist()→rmqueue()→page_returned→set_pte_at()→update_PML4/PDPT/PD/PT→user_gets_virtual_address. 100 processes = 100 independent paths, same function chain.
157. Q(line54-55): WHY GET INCREMENTS AND PUT DECREMENTS? NAMING CONFUSION. get_page = "I am getting/acquiring this page" → refcount++ (I am now a user). put_page = "I am putting back/releasing this page" → refcount-- (I am no longer a user). NOT get=read, put=write. Think: get=take, put=give_back.

---

## PART 6: BUDDY ALLOCATOR (FROM SCRATCH)
---

158. Q(line56): WHAT IS BUDDY ALLOCATOR? FROM SCRATCH DERIVATION:
159. AXIOM: RAM on this machine = 16154906624 bytes (line 08). Pages = 3944069 (line 20). Each page = 4096 bytes (line 12).
160. PROBLEM: Kernel needs 8 contiguous pages. How to find them fast?
161. NAIVE SOLUTION: Search all 3944069 pages one by one. Trace:
162. Step 1: Check page[0]. Free? Yes. Counter=1.
163. Step 2: Check page[1]. Free? Yes. Counter=2.
164. ...
165. Step N: Check page[N]. Free? No. Counter reset to 0. Start over.
166. Worst case: Check all 3944069 pages. Steps = 3944069. SLOW.
167. DEFINITION: STEPS = number of operations CPU performs. More steps = more time.
168. QUESTION: Can we reduce steps? From 3944069 to something smaller?
169. INSIGHT: If we ALREADY KNOW where 8-page blocks are, we don't need to search.
170. SOLUTION: BEFORE anyone asks, group pages by size. Store POINTERS to groups.
171. DEFINITION: LIST = chain of items. Each item points to next item. First item called HEAD.
172. DRAW: HEAD → [block_0] → [block_1] → [block_2] → ... → [block_N] → NULL
173. DEFINITION: free_area[order] = list of free blocks of size 2^order pages.
174. DRAW: free_area[0] = list of 1-page blocks. free_area[1] = list of 2-page blocks. ... free_area[10] = list of 1024-page blocks.
175. ALLOCATION WITH LISTS: Need 8 pages. 8 = 2^3. Look at free_area[3].
176. Step 1: Check free_area[3].HEAD. Not NULL? Block exists. Take it.
177. Steps = 1. Compare: naive = 3944069 steps. Lists = 1 step. Speedup = 3944069 / 1 = 3944069×.
178. COST OF LISTS: Must maintain 11 lists. When page freed, add to list. When page allocated, remove from list. CONSTANT time operations.
179. DEFINITION: ORDER = index of free_area array. order=0 → blocks of 2^0=1 page. order=1 → blocks of 2^1=2 pages. order=N → blocks of 2^N pages.
164. CALCULATION: 2^0=1. 2^1=2. 2^2=4. 2^3=8. 2^4=16. 2^5=32. 2^6=64. 2^7=128. 2^8=256. 2^9=512. 2^10=1024. ✓
165. AXIOM: MAX_ORDER=11 in Linux kernel. Largest block = 2^(11-1) = 2^10 = 1024 pages = 1024×4096 = 4194304 bytes = 4 MB.
166. SOURCE: cat /proc/buddyinfo → output for this machine RIGHT NOW:
167. DMA:    0   1   0   0   0   0   0   0   1   2   2
168. DMA32:  25048 17037 9805 4738 1572 406 90 14 10 26 123
169. Normal: 25281 22621 4377 854 383 177 78 40 9 6 3
170. MEANING: Each column = one order. Column 0 = order 0. Column 1 = order 1. ... Column 10 = order 10.
171. DRAW Normal zone: order[0]=25281 blocks × 1 page = 25281 pages. order[1]=22621 blocks × 2 pages = 45242 pages. order[2]=4377 blocks × 4 pages = 17508 pages. ...
172. CALCULATION: Total free pages in Normal zone:
173. order[0]: 25281 × 2^0 = 25281 × 1 = 25281
174. order[1]: 22621 × 2^1 = 22621 × 2 = 45242
175. order[2]: 4377 × 2^2 = 4377 × 4 = 17508
176. order[3]: 854 × 2^3 = 854 × 8 = 6832
177. order[4]: 383 × 2^4 = 383 × 16 = 6128
178. order[5]: 177 × 2^5 = 177 × 32 = ?. CALCULATE: 177×30=5310. 177×2=354. 5310+354=5664. ✓
179. order[6]: 78 × 2^6 = 78 × 64 = ?. CALCULATE: 78×60=4680. 78×4=312. 4680+312=4992. ✓
180. order[7]: 40 × 2^7 = 40 × 128 = ?. CALCULATE: 40×100=4000. 40×28=1120. 4000+1120=5120. ✓
181. order[8]: 9 × 2^8 = 9 × 256 = ?. CALCULATE: 9×256=9×200+9×56=1800+504=2304. ✓
182. order[9]: 6 × 2^9 = 6 × 512 = ?. CALCULATE: 6×500=3000. 6×12=72. 3000+72=3072. ✓
183. order[10]: 3 × 2^10 = 3 × 1024 = ?. CALCULATE: 3×1000=3000. 3×24=72. 3000+72=3072. ✓
184. TOTAL NORMAL: 25281+45242+17508+6832+6128+5664+4992+5120+2304+3072+3072 = ?
185. STEP: 25281+45242=70523. 70523+17508=88031. 88031+6832=94863. 94863+6128=100991. 100991+5664=106655. 106655+4992=111647. 111647+5120=116767. 116767+2304=119071. 119071+3072=122143. 122143+3072=125215. TOTAL=125215 pages.
186. VERIFY BYTES: 125215×4096=?. CALCULATE: 125215×4000=500860000. 125215×96=12020640. 500860000+12020640=512880640 bytes.
187. VERIFY MB: 512880640÷1048576=?. CALCULATE: 512880640÷1048576=489.17 MB. ≈489 MB free.
188. ---
189. PUNISHMENT CALCULATION: HARD EXAMPLE
190. ---
191. PROBLEM: DMA32 zone has order[10]=123 blocks. Each block=1024 pages. Total pages at order[10]=?
192. CALCULATE: 123×1024=?. 123×1000=123000. 123×24=2952. 123000+2952=125952 pages at order[10] in DMA32.
193. BYTES: 125952×4096=?. CALCULATE: 125952×4000=503808000. 125952×96=12091392. 503808000+12091392=515899392 bytes.
194. MB: 515899392÷1048576=?. CALCULATE: 515899392÷1000000=515.899. Adjust for 1048576: 515899392÷1048576=492.06 MB at order[10].
195. COMPARE: Normal zone order[10]=3 blocks=3072 pages=12582912 bytes=12 MB. DMA32 order[10]=123 blocks=125952 pages=515899392 bytes=492 MB. Ratio=492/12=41×.
196. ---
197. EDGE CASE: SPLITTING CALCULATION
198. ---
199. PROBLEM: Need 1 page (order=0). order[0] list empty. order[1] has 1 block (2 pages). What happens?
200. STEP1: Check order[0]. count=0. Empty.
201. STEP2: Check order[1]. count=1. Take block. Block has 2 pages at PFN=1000.
202. STEP3: Split. 2 pages→1 page + 1 page. Give PFN=1000 to caller. Put PFN=1001 in order[0] list.
203. AFTER: order[0] count=1. order[1] count=0.
204. ---
205. HARDER EDGE: MULTI-LEVEL SPLIT
206. ---
207. PROBLEM: Need 1 page. order[0]=0. order[1]=0. order[2]=0. order[3]=1 block at PFN=1000 (8 pages).
208. STEP1: order[0]=0. Check order[1]=0. Check order[2]=0. Check order[3]=1.
209. STEP2: Take order[3] block. 8 pages at PFN=1000-1007.
210. STEP3: Split→4+4. Give first 4 (PFN=1000-1003) to order[2]? NO. We need order[0]. Keep splitting.
211. STEP4: 4 pages at PFN=1000-1003 splits→2+2. Put PFN=1002-1003 in order[1]. Continue with PFN=1000-1001.
212. STEP5: 2 pages at PFN=1000-1001 splits→1+1. Put PFN=1001 in order[0]. Give PFN=1000 to caller.
213. STEP6: Remaining 4 pages at PFN=1004-1007 goes to order[2].
214. AFTER: order[0]=1 (PFN=1001). order[1]=1 (PFN=1002-1003). order[2]=1 (PFN=1004-1007). order[3]=0.
215. VERIFY: 1+2+4+0=7 pages in lists. 1 page given to caller. 7+1=8 pages. Original order[3]=8 pages. ✓
216. ---
217. DEFINITION: WHAT IS A BUDDY? (FROM SCRATCH)
218. ---
219. NEW SCENARIO (unrelated to lines 207-215): We now explore FREEING, not allocating.
220. AXIOM: System has been running. Many allocations and frees happened. Current state varies.
221. SCENARIO A (buddy NOT free): Before free, order[3].nr_free=0. All 8-page blocks are in use.
222. DRAW STATE A BEFORE: free_area[3] = { head→NULL, nr_free=0 }. Block PFN=1000 is IN USE. Block PFN=992 is IN USE.
223. SCENARIO B (buddy IS free): Before free, order[3].nr_free=1. One 8-page block (PFN=992) is free.
224. DRAW STATE B BEFORE: free_area[3] = { head→[PFN=992]→NULL, nr_free=1 }. Block PFN=1000 is IN USE. Block PFN=992 is FREE.
225. NOW: User frees block at PFN=1000 (8 pages). What happens? Depends on scenario.
226. PROBLEM: Block at PFN=1000, order=3 (8 pages) is freed. Where to put it?
227. NAIVE: Put in free_area[3] list. Done. PROBLEM: Fragmentation over time.
228. BETTER: FIRST check if buddy is free. If yes, merge. If no, just add to list.
222. QUESTION: What does ADJACENT mean for blocks?
223. ANSWER: Two blocks are adjacent if one ends where the other starts. No gap. No overlap.
224. EXAMPLE: Block A = PFN 1000-1007 (8 pages). Block B = PFN 1008-1015 (8 pages). Adjacent? 1007+1=1008. ✓ Adjacent.
225. EXAMPLE: Block A = PFN 1000-1007. Block C = PFN 992-999 (8 pages). Adjacent? 999+1=1000. ✓ Adjacent.
226. OBSERVATION: Block at PFN=1000 has TWO adjacent blocks: one BEFORE (992-999), one AFTER (1008-1015).
227. QUESTION: Which adjacent block is the BUDDY? Left one (992) or right one (1008)?
228. ANSWER: Only ONE is the buddy. The other is NOT. Why?
229. REASON: Merged block must be ALIGNED. Alignment = starting PFN divisible by merged block size.
230. MERGED SIZE: Two 8-page blocks merge into one 16-page block (order=4).
231. ALIGNMENT REQUIREMENT: 16-page block must start at PFN divisible by 16.
232. CHECK PFN=1000: 1000 ÷ 16 = 62.5. Not integer. 1000 is NOT 16-aligned.
233. CHECK PFN=992: 992 ÷ 16 = 62. Integer. 992 IS 16-aligned. ✓
234. CHECK PFN=1008: 1008 ÷ 16 = 63. Integer. 1008 IS 16-aligned. ✓
235. PROBLEM: Both 992 and 1008 are 16-aligned. Which forms valid parent with 1000?
236. PARENT COVERAGE: 16-page block at PFN=992 covers 992-1007. Does it include 1000-1007? 1000≥992 ✓. 1007<1008 ✓. YES.
237. PARENT COVERAGE: 16-page block at PFN=1008 covers 1008-1023. Does it include 1000-1007? 1000≥1008? NO. ✗
238. CONCLUSION: Block 992-999 is BUDDY of block 1000-1007. Block 1008-1015 is NOT buddy.
239. DEFINITION: BUDDY = the OTHER half of the SAME parent block.
240. DRAW PARENT: 16-page parent at PFN=992:
241. |PFN 992|993|994|995|996|997|998|999|1000|1001|1002|1003|1004|1005|1006|1007|
242. |<-------- FIRST HALF = BUDDY -------->|<-------- SECOND HALF = OUR BLOCK -->|
243. |<-------- 8 pages at PFN=992 -------->|<-------- 8 pages at PFN=1000 ------->|
244. QUESTION: How many buddies does a block have?
245. ANSWER: Exactly ONE. Because each block belongs to exactly one parent. Parent splits into exactly two halves.
246. QUESTION: Is buddy in SAME gang or DIFFERENT gang?
247. CLARIFY: "Gang" = free_area[N] list. Block at order=3 is in free_area[3] gang.
248. ANSWER: Buddy is in SAME gang (same order). Both have size 2^3=8 pages. Both are (or were) in free_area[3].
249. QUESTION: How to find buddy PFN mathematically?
250. OBSERVATION: Parent starts at 16-aligned PFN. Two children: first half at parent_pfn, second half at parent_pfn+8.
251. OBSERVATION: First half has bit 3 CLEAR (divisible by 16). Second half has bit 3 SET (not divisible by 16).
252. FORMULA INSIGHT: Buddy = same parent, opposite half. Flip bit at position ORDER to toggle halves.
253. FORMULA: buddy_pfn = pfn XOR (1 << order). XOR flips bit at position ORDER.
254. VERIFY: pfn=1000, order=3. 1<<3=8. buddy=1000 XOR 8.
255. ---
256. BIT PATTERN CALCULATION: BUDDY XOR (NOW WITH CONTEXT)
257. ---
258. PROBLEM: PFN=1000, order=3. Find buddy using XOR. Verify with bits.
220. PFN=1000 in binary: 1000÷2=500r0. 500÷2=250r0. 250÷2=125r0. 125÷2=62r1. 62÷2=31r0. 31÷2=15r1. 15÷2=7r1. 7÷2=3r1. 3÷2=1r1. 1÷2=0r1. Read bottom-up: 1111101000.
221. VERIFY: 1111101000 = 512+256+128+64+32+0+8+0+0+0 = 512+256+128+64+32+8 = 1000. ✓
222. XOR MASK: 1<<3 = 8 = 1000 in binary = 0000001000 (10 bits).
223. XOR: 1111101000 XOR 0000001000 = ?. Bit by bit: 1^0=1. 1^0=1. 1^0=1. 1^0=1. 1^0=1. 0^0=0. 1^1=0. 0^0=0. 0^0=0. 0^0=0. Result: 1111100000.
224. CONVERT: 1111100000 = 512+256+128+64+32+0+0+0+0+0 = 992. Buddy PFN=992. ✓
225. REVERSE: buddy=992, order=3. 992 XOR 8 = ?. 992=1111100000. 8=0000001000. XOR=1111101000=1000. ✓
226. ---
227. SCENARIO A TRACE (buddy NOT free): Buddy PFN=992. Is 992 in free_area[3] list?
228. STATE A BEFORE: free_area[3] = { head→NULL, nr_free=0 }. List is EMPTY.
229. CHECK: Is 992 in empty list? NO. Buddy is NOT free.
230. ACTION: Cannot merge. Just add PFN=1000 to list.
231. STATE A AFTER: free_area[3] = { head→[PFN=1000]→NULL, nr_free=1 }.
232. RESULT A: No merge. 1 block of 8 pages in order[3].
233. ---
234. SCENARIO B TRACE (buddy IS free): Buddy PFN=992. Is 992 in free_area[3] list?
235. STATE B BEFORE: free_area[3] = { head→[PFN=992]→NULL, nr_free=1 }. Block 992 IS in list.
236. CHECK: Is 992 in list? YES. Buddy is FREE.
237. ACTION: Remove 992 from list. Merge 992+1000 into 16-page block at PFN=992.
238. STATE B AFTER order[3]: free_area[3] = { head→NULL, nr_free=0 }. Both blocks removed, merged.
239. NEXT: Merged block (PFN=992, order=4) must be added to order[4]. But first, check order[4] buddy.
240. RECURSE: buddy of (992,order=4) = 992 XOR 16 = 1008. Is 1008 in free_area[4]? If yes→merge again. If no→add to order[4].
241. RESULT B: Merge happened. order[3] emptied. 1 block of 16 pages moved to order[4] (or higher if more merges).
242. ---
227. FRACTIONAL EDGE: NON-POWER-OF-2 REQUEST
228. ---
229. PROBLEM: User needs 5 pages. 5 is NOT power of 2. What order?
230. CALCULATE: 2^2=4<5. 2^3=8≥5. Must use order=3 (8 pages). Waste=8-5=3 pages. Fragmentation.
231. PROBLEM: User needs 1000 pages. What order?
232. CALCULATE: 2^9=512<1000. 2^10=1024≥1000. Must use order=10. Waste=1024-1000=24 pages.
233. PROBLEM: User needs 1025 pages. What order?
234. CALCULATE: 2^10=1024<1025. 2^11=2048≥1025. But MAX_ORDER=11→max block=1024. CANNOT allocate 1025 contiguous pages with buddy.
235. SOLUTION: Allocate 2 blocks: order[10]+order[0]. But NOT contiguous. Caller must handle.
236. ---
237. SCALE STRESS TEST: ALL ZONES
238. ---
239. DMA zone total pages: 0×1+1×2+0×4+0×8+0×16+0×32+0×64+0×128+1×256+2×512+2×1024=0+2+0+0+0+0+0+0+256+1024+2048=3330 pages.
240. DMA32 zone total: 25048×1+17037×2+9805×4+4738×8+1572×16+406×32+90×64+14×128+10×256+26×512+123×1024=?
241. STEP: 25048. 17037×2=34074. 25048+34074=59122.
242. STEP: 9805×4=39220. 59122+39220=98342.
243. STEP: 4738×8=37904. 98342+37904=136246.
244. STEP: 1572×16=25152. 136246+25152=161398.
245. STEP: 406×32=12992. 161398+12992=174390.
246. STEP: 90×64=5760. 174390+5760=180150.
247. STEP: 14×128=1792. 180150+1792=181942.
248. STEP: 10×256=2560. 181942+2560=184502.
249. STEP: 26×512=13312. 184502+13312=197814.
250. STEP: 123×1024=125952. 197814+125952=323766 pages in DMA32.
251. TOTAL ALL ZONES: 3330+323766+125215=452311 pages free on this machine.
252. BYTES: 452311×4096=1852265456 bytes=1.72 GB free RAM.
183. order[10]: 3 × 2^10 = 3 × 1024 = 3072
184. TOTAL: 25281+45242+17508+6832+6128+5664+4992+5120+2304+3072+3072 = 125215 free pages in Normal zone.
185. VERIFY: 125215 × 4096 = 512880640 bytes = 489 MB free in Normal zone.
186. ALLOCATION EXAMPLE: Need 8 pages. 8 = 2^3 → order = 3. Look at order[3] list in Normal zone. Count = 854. Take 1 block. Now count = 853.
187. DRAW BEFORE: free_area[3].nr_free = 854 → [block_0][block_1]...[block_853]
188. DRAW AFTER: free_area[3].nr_free = 853 → [block_1][block_2]...[block_853]. block_0 given to caller.
189. SPLITTING: Need 8 pages but order[3]=0 (empty). Look at order[4]. Take 1 block (16 pages). SPLIT: 16 pages → 8 pages + 8 pages. Give 8 to caller. Put 8 in order[3] list.
190. DRAW: order[4] has [16-page block] → take → split → [8-page block to caller] + [8-page block to order[3]]
191. MERGING (on free): FROM SCRATCH DERIVATION:
192. AXIOM: order[N] list contains blocks of 2^N pages. order[3] = blocks of 8 pages. order[4] = blocks of 16 pages.
193. PROBLEM: User frees 8 pages at PFN=1000. Where to put them?
194. NAIVE: Put in order[3] list. Done. PROBLEM: Over time, many small blocks, few large blocks. Request for 16 pages fails even if enough total free pages exist.
195. BETTER: Check if ADJACENT 8-page block is also free. If yes, MERGE into 16-page block. Put in order[4] list.
196. QUESTION: What is ADJACENT? Which PFN is adjacent to PFN=1000?
197. WRONG ANSWER: PFN=1001 is adjacent. NO! PFN=1000 is START of 8-page block. Block covers PFN 1000-1007. PFN=1001 is INSIDE this block.
198. CORRECT: For order[3] block at PFN=1000, the 8 pages are: 1000, 1001, 1002, 1003, 1004, 1005, 1006, 1007.
199. ADJACENT BLOCK: Starts at PFN=1008 or PFN=992 (the 8 pages BEFORE).
200. QUESTION: Which one? 1008 or 992? Answer: BUDDY formula tells us.
201. DEFINITION: BUDDY = the OTHER half of the PARENT block. Parent = block of 16 pages (order[4]). Parent must be ALIGNED.
202. DEFINITION: ALIGNED block = block whose starting PFN is divisible by block size. order[4] block (16 pages) must start at PFN divisible by 16.
203. CALCULATION: Is PFN=1000 aligned for order[4]? 1000 ÷ 16 = 62.5. NOT integer. ∴ 1000 is NOT aligned for 16-page block.
204. CALCULATION: What is nearest 16-aligned PFN ≤ 1000? 1000 ÷ 16 = 62 (floor). 62 × 16 = 992. ∴ PFN=992 is 16-aligned.
205. CONSEQUENCE: If we want to merge block at PFN=1000 into 16-page block, parent must start at PFN=992 (since 992 is 16-aligned and 992+16=1008 covers 1000).
206. VERIFY: Parent block at PFN=992 covers pages 992-1007. Our block at PFN=1000 covers pages 1000-1007. INSIDE parent? 1000 ≥ 992 ✓. 1007 < 1008 ✓. Yes, our block is SECOND HALF of parent.
207. BUDDY: FIRST HALF of parent = PFN=992, pages 992-999. This is our BUDDY.
208. FORMULA DERIVATION: How to compute buddy PFN mathematically?
209. OBSERVATION: Order[3] blocks start at PFN divisible by 8. Order[4] blocks start at PFN divisible by 16.
210. OBSERVATION: Two order[3] buddies share same parent. One has bit 3 (value 8) CLEAR in PFN. One has bit 3 SET.
211. EXAMPLE: PFN=992 in binary = 1111100000. PFN=1000 in binary = 1111101000. Difference: bit 3 (counting from 0).
212. CALCULATION: 992 = 1111100000. 1000 = 1111101000. XOR: 1111100000 XOR 1111101000 = 0000001000 = 8 = 2^3 = (1 << 3). ✓
213. FORMULA: buddy_pfn = pfn XOR (1 << order). For order=3: buddy = pfn XOR 8.
214. VERIFY: pfn=1000, order=3. buddy = 1000 XOR 8. Binary: 1111101000 XOR 0000001000 = 1111100000 = 992. ✓
215. VERIFY: pfn=992, order=3. buddy = 992 XOR 8. Binary: 1111100000 XOR 0000001000 = 1111101000 = 1000. ✓ (Symmetric!)
216. WHY XOR WORKS: XOR flips the bit at position ORDER. This toggles between first and second half of parent block.
217. DRAW: 16-page parent at PFN=992:
218. [PFN 992][993][994][995][996][997][998][999] | [1000][1001][1002][1003][1004][1005][1006][1007]
219. [----------- 8 pages, bit3=0 ----------]   | [----------- 8 pages, bit3=1 ----------------]
220. [----------- buddy of 1000 ------------]   | [----------- block at PFN=1000 --------------]
221. MERGE ALGORITHM: free(page at PFN=1000, order=3):
222. Step1: buddy_pfn = 1000 XOR 8 = 992.
223. Step2: Is page at PFN=992 free? Check free_area[3] list. If 992 in list → buddy is free.
224. Step3: If buddy free → remove buddy from order[3] list → merged_pfn = min(1000, 992) = 992 → merged_order = 3+1 = 4 → recurse: free(992, order=4).
225. Step4: If buddy NOT free → put (1000, order=3) in free_area[3] list → done.
226. RECURSE EXAMPLE: free(992, order=4):
227. buddy_pfn = 992 XOR 16 = 992 XOR 10000 (binary). 992 = 1111100000. 16 = 10000. XOR = 1111110000 = 1008. buddy = 1008.
228. Is 1008 free at order[4]? If yes → merge into order[5] block at PFN=992 (since 992 is 32-aligned: 992/32=31). If no → stop, put (992, order=4) in list.
229. VERIFY ALIGNMENT: 992 ÷ 32 = 31. 31 is integer. ✓ 992 is 32-aligned.
230. MAX_ORDER STOP: Recursion stops at order=10. Cannot merge order[10] blocks. Largest block = 1024 pages = 4MB.
197. Q(line57): CHECKS FOR DOUBLE-FREE? ANSWER: YES, kernel has checks. VM_BUG_ON_PAGE(page_ref_count(page) <= 0, page) → if refcount ≤ 0 → BUG(). User-space free() calls kernel munmap() → kernel checks page_mapcount/refcount BEFORE decrementing. User-space cannot directly call put_page(). Only kernel can. Kernel assumes kernel code is correct. Kernel driver bugs CAN cause double-free → panic.
198. Q(line58-67): WHY GFP FLAGS? FROM SCRATCH DERIVATION:
199. AXIOM: CPU executes instructions one at a time. Current instruction address stored in register (RIP on x86_64).
200. DEFINITION: PROCESS = running program. Process has: code, data, stack, registers. Kernel tracks each process in task_struct.
201. DEFINITION: CONTEXT = which code is currently running. Two types: PROCESS CONTEXT = running on behalf of a process (syscall, normal code). INTERRUPT CONTEXT = running on behalf of hardware event.
202. PROBLEM: alloc_page() sometimes fails. RAM is full. What to do?
203. OPTION 1: Return NULL immediately. Caller handles failure. FAST but FAILS often.
204. OPTION 2: Wait. Free some RAM. Try again. SLOWER but SUCCEEDS more.
205. QUESTION: How to free RAM? WHERE does free RAM come from?
206. AXIOM: Kernel uses RAM for: (A) process pages (code, heap, stack), (B) page cache (recently read files), (C) slab cache (kernel objects), (D) buffers.
207. DEFINITION: RECLAIM = kernel takes RAM from (B), (C), (D) when needed. Pages in page cache can be DROPPED if not dirty. Dirty pages must be written to disk first.
208. PROBLEM: Writing to disk takes TIME. Disk I/O = milliseconds. CPU cycle = nanoseconds. Ratio: 1ms / 1ns = 1,000,000×.
209. DEFINITION: SLEEP = process stops running, waits for event (disk I/O complete). Kernel switches to another process. CANNOT sleep if no process to switch to.
210. PROBLEM: In INTERRUPT CONTEXT, there is no process. Interrupt handler runs BETWEEN process time slices. If interrupt handler sleeps → DEADLOCK. No one to wake it up.
211. CONSEQUENCE: Interrupt handler CANNOT sleep. Cannot wait for disk I/O. Cannot reclaim memory that requires disk write.
212. SOLUTION: Tell allocator what is ALLOWED. Use FLAGS.
213. DEFINITION: GFP = Get Free Pages. GFP flags = bitmask telling allocator: can you sleep? can you do I/O? can you access filesystem?
214. AXIOM: GFP_ATOMIC = 0x0 = no bits set = cannot sleep, cannot do I/O, cannot use FS. Used in interrupt context.
215. AXIOM: GFP_KERNEL = 0xCC0 = multiple bits set = can sleep, can do I/O, can use FS. Used in process context.
216. DERIVATION OF 0xCC0 (repeating from lines 62-67 with more detail):
217. AXIOM: Kernel defines bit positions as enum in include/linux/gfp_types.h line 27-38.
218. DRAW: enum { ___GFP_DMA_BIT=0, ___GFP_HIGHMEM_BIT=1, ___GFP_DMA32_BIT=2, ___GFP_MOVABLE_BIT=3, ___GFP_RECLAIMABLE_BIT=4, ___GFP_HIGH_BIT=5, ___GFP_IO_BIT=6, ___GFP_FS_BIT=7, ... ___GFP_DIRECT_RECLAIM_BIT=10, ___GFP_KSWAPD_RECLAIM_BIT=11, ... }
219. CALCULATION: ___GFP_IO = BIT(6) = 1 << 6 = 2^6 = 64. Binary: 0000 0100 0000. Hex: 0x40.
220. CALCULATION: ___GFP_FS = BIT(7) = 1 << 7 = 2^7 = 128. Binary: 0000 1000 0000. Hex: 0x80.
221. CALCULATION: ___GFP_DIRECT_RECLAIM = BIT(10) = 1 << 10 = 2^10 = 1024. Binary: 0100 0000 0000. Hex: 0x400.
222. CALCULATION: ___GFP_KSWAPD_RECLAIM = BIT(11) = 1 << 11 = 2^11 = 2048. Binary: 1000 0000 0000. Hex: 0x800.
223. DEFINITION: __GFP_RECLAIM = ___GFP_DIRECT_RECLAIM | ___GFP_KSWAPD_RECLAIM. Source: grep __GFP_RECLAIM gfp_types.h.
224. CALCULATION: __GFP_RECLAIM = 0x400 | 0x800. Binary OR: 0100 0000 0000 | 1000 0000 0000 = 1100 0000 0000 = 0xC00 = 3072.
225. DEFINITION: GFP_KERNEL = __GFP_RECLAIM | __GFP_IO | __GFP_FS. Source: grep GFP_KERNEL gfp_types.h.
226. CALCULATION: GFP_KERNEL = 0xC00 | 0x40 | 0x80. Step: 0xC00 = 1100 0000 0000. 0x40 = 0000 0100 0000. 0x80 = 0000 1000 0000. OR all: 1100 1100 0000 = 0xCC0 = 3264.
227. VERIFY: 3264 = 3072 + 64 + 128 = 3072 + 192 = 3264. ✓
228. MEANING OF EACH BIT:
229. bit 6 (__GFP_IO): Allocator CAN start disk I/O. If unset: allocator cannot touch disk.
230. bit 7 (__GFP_FS): Allocator CAN call filesystem functions. If unset: no FS calls (avoid deadlock if already holding FS lock).
231. bit 10 (__GFP_DIRECT_RECLAIM): Allocator CAN directly reclaim pages (caller's context). Will SLEEP waiting.
232. bit 11 (__GFP_KSWAPD_RECLAIM): Allocator CAN wake kswapd.
233. DEFINITION: kswapd = kernel thread. Runs in background. Reclaims memory when free pages low. One kswapd per NUMA node.
234. DRAW: alloc_page(GFP_KERNEL=0xCC0) → check free list → if empty → bit 10 set → try direct reclaim → sleep while pages freed → retry → success.
235. DRAW: alloc_page(GFP_ATOMIC=0x0) → check free list → if empty → no bits set → cannot reclaim → return NULL immediately.
236. EXAMPLE SCENARIO: Driver in interrupt handler needs 1 page. Uses GFP_ATOMIC. Free list empty. alloc_page returns NULL. Driver must handle failure. Cannot wait.
237. EXAMPLE SCENARIO: Process calls mmap(). Kernel needs page. Uses GFP_KERNEL. Free list empty. Kernel reclaims page cache. Process sleeps 1ms. Page freed. alloc_page succeeds.
238. WHY FLAGS AT BOOT? At boot: all RAM free. Buddy allocator full. alloc_page succeeds immediately. GFP flags ignored. At runtime: RAM used. alloc_page may fail. GFP flags control fallback behavior.
199. Q(line68): ALLOCATES FROM WHERE? ANSWER: Buddy allocator maintains FREE LISTS. Boot: memblock gives all RAM to buddy → buddy builds free lists. alloc_page() → removes page from free list → gives to caller. NOT "already allocated at boot". Boot ORGANIZES, runtime DISTRIBUTES. DRAW: boot→[RAM=all_pages]→memblock→[buddy_free_lists]→runtime→alloc_page()→[page removed from list]→put_page()→[page returned to list].
200. Q(line70): WHAT IF TWO CALL PAGE_REF_COUNT TOGETHER? ANSWER: page_ref_count() uses atomic_read(). atomic_read() is LOCK-FREE read on x86. Multiple CPUs can read simultaneously. No race condition for READ. For WRITE (get_page/put_page): uses atomic_inc/atomic_dec → CPU instruction LOCK prefix → hardware ensures atomicity.
201. Q(line71): BUT ALLOC_PAGE ALREADY SET REFCOUNT=1? ANSWER: get_page() is for SECOND user. SCENARIO: Process_A calls alloc_page() → refcount=1. Process_A shares page with Process_B → get_page() → refcount=2. Both A and B now have reference. When A done → put_page() → refcount=1. When B done → put_page() → refcount=0 → freed. WITHOUT get_page(): A shares with B, A calls put_page() → refcount=0 → FREED while B still using → CRASH.
202. Q(line72): WHAT IF PUT_PAGE ON ILLEGAL PAGE? ANSWER: put_page(NULL) → NULL pointer dereference → kernel OOPS. put_page(invalid_address) → page_ref_dec_and_test reads garbage → undefined behavior. put_page(freed_page) → refcount goes negative OR corrupts re-allocated page → BUG_ON or silent corruption. Kernel does NOT validate page pointer. Assume caller is correct. Driver bug = kernel bug = panic.
203. Q(line77): TYPECAST AND CR3 RELATION? DRAW: pfn=123→phys=123×4096=503808=0x7B000→this_is_RAM_bus_address. (phys_addr_t) = cast to unsigned long long = 64-bit integer. phys is JUST A NUMBER, not a pointer. CANNOT dereference phys directly. To access RAM at phys: virt=__va(phys)=phys+PAGE_OFFSET=0x7B000+0xFFFF888000000000=0xFFFF88800007B000 → NOW can dereference. CR3 relation: CR3 register holds ROOT of page tables (PML4). Page table entries contain PFN. PFN×4096=physical_address_of_next_table_or_page. MMU uses PFN, kernel uses PFN, allocator tracks PFN.
204. Q(line78): TYPE OF REF? ANSWER: ref type = int = 4 bytes = signed 32-bit. page_ptr type = struct page * = 8 bytes = pointer. WHY care? You are learning what kernel tracks. ref=1 means 1 user. ref=0 means free. Type tells you range: int can be -2B to +2B. refcount > 2B users = impossible = overflow check.
205. Q(line79): PFN RANGE VS PML4 INDEXING? THESE ARE DIFFERENT THINGS. PML4 indexing: virtual_address[47:39]=PML4_index, [38:30]=PDPT_index, [29:21]=PD_index, [20:12]=PT_index, [11:0]=offset. This gives VIRTUAL address translation. PFN and zones: physical_address/4096=PFN. Zone tells allocator WHERE in RAM. RELATION: PT_entry contains PFN. DRAW: user_virtual_0x7FFFFFFF000→PML4[255]→PDPT[511]→PD[511]→PT[4095]→PTE_contains_PFN=123→physical=0x7B000. Zone of PFN 123: 123<4096→ZONE_DMA. PML4 indexing finds physical page. Zone tells allocator which freelist to use.
206. ---
207. PROGRESS STATUS
208. ---
209. DONE: worksheet 237 lines. DONE: bootmem_trace.c compiled. DONE: questions answered with from-scratch derivations.
210. PENDING: insmod bootmem_trace.ko → dmesg → verify refcount transitions.
211. PENDING: uncomment bug line → trigger refcount=-1 → observe kernel warning.
212. PENDING: git push.
213. ---

---

## PART 7: BUDDY ALGORITHM (FREE OPERATION & XOR TRICK)
---

214. ===
215. BUDDY ALLOCATOR COMPLETE ALGORITHM (DERIVED FROM FREE PROBLEM)
216. ===
217. FACT: User finished using 8 pages at PFN 1572864, 1572865, 1572866, 1572867, 1572868, 1572869, 1572870, 1572871.
218. FACT: User calls __free_pages(page_ptr, 3). Kernel must return these 8 pages.
219. QUESTION: Where does kernel put these 8 pages?
220. ---
221. OPTION 1: JUST ADD TO LIST
222. ACTION: Add block (PFN=1572864, 8 pages) to free_area[3] list.
223. RESULT: free_area[3].nr_free increases by 1.
224. PROBLEM: Missed opportunity to create larger block.
225. ---
226. THE PROBLEM WITH OPTION 1
227. SCENARIO: Before free, state was:
228. free_area[3] contains: [PFN=1572872] (1 block, 8 pages).
229. free_area[4] contains: [] (0 blocks).
230. OBSERVATION: PFN 1572872-1572879 is FREE. PFN 1572864-1572871 is being freed.
231. DRAW:
232. PFN: 1572864 1572865 1572866 1572867 1572868 1572869 1572870 1572871 | 1572872 1572873 1572874 1572875 1572876 1572877 1572878 1572879
233.      [------- 8 pages being freed (user returning) ---------------] | [------- 8 pages already free (in list) ---------------------]
234.      [------- BLOCK A -----------------------------------------] | [------- BLOCK B -----------------------------------------]
235. OBSERVATION: Block A and Block B are ADJACENT. Combined = 16 contiguous pages.
236. ---
237. WHY COMBINING IS GOOD
238. FUTURE REQUEST: Someone asks for 16 pages (order=4).
239. IF NOT COMBINED: free_area[4] = 0 blocks. FAIL.
240. IF COMBINED: free_area[4] = 1 block. SUCCESS.
241. CONCLUSION: Combining adjacent blocks allows larger future allocations.
242. ---
243. QUESTION: WHICH ADJACENT BLOCK TO COMBINE WITH?
244. Block at PFN=1572864 has TWO adjacent 8-page blocks:
245. BEFORE: PFN 1572856-1572863.
246. AFTER: PFN 1572872-1572879.
247. WHICH ONE?
248. ---
249. CHECKING BEFORE BLOCK (PFN 1572856-1572863)
250. IF COMBINED: 1572856-1572863 + 1572864-1572871 = 16 pages at PFN 1572856.
251. RULE: 16-page block must start at PFN divisible by 16.
252. CHECK: 1572856 ÷ 16 = 98303.5. NOT integer.
253. CONCLUSION: CANNOT combine with 1572856. Wrong alignment.
254. ---
255. CHECKING AFTER BLOCK (PFN 1572872-1572879)
256. IF COMBINED: 1572864-1572871 + 1572872-1572879 = 16 pages at PFN 1572864.
257. CHECK: 1572864 ÷ 16 = 98304. INTEGER.
258. CONCLUSION: CAN combine with 1572872. Correct alignment.
259. ---
260. WHY ONE WORKS AND OTHER DOESN'T
261. DRAW 16-PAGE BOUNDARIES:
262. PFN:       1572848 -------- 1572863 | 1572864 -------- 1572879 | 1572880 -------- 1572895
263.            [---- 16 pages --------] | [---- 16 pages --------] | [---- 16 pages --------]
264. 1572864 is ON a 16-page boundary. 1572856 is NOT.
265. RULE: Can only combine into blocks that start ON boundaries.
266. ---
267. DEFINITION: PARENT
268. PARENT = the 16-page block that contains our 8-page block.
269. Our block: 1572864-1572871.
270. Parent: 1572864-1572879 (16 pages, starts at 1572864 which is 16-aligned).
271. ---
272. DEFINITION: BUDDY
273. BUDDY = the OTHER half of the same parent.
274. Parent: 1572864-1572879.
275. First half: 1572864-1572871 (our block).
276. Second half: 1572872-1572879 (the buddy).
277. NAME REASON: They can combine. They are partners.
278. ---
279. FINDING BUDDY MATHEMATICALLY
280. Our PFN: 1572864.
281. Parent must be 16-aligned. 1572864 ÷ 16 = 98304. Integer. Parent starts at 1572864.
282. Parent covers: 1572864 to 1572864+16-1 = 1572879.
283. We are: first 8 pages.
284. Buddy is: last 8 pages. Buddy PFN = 1572864 + 8 = 1572872.
285. ---
286. ALTERNATE CASE: WHAT IF WE HAD PFN 1572872?
287. Our PFN: 1572872.
288. Parent must be 16-aligned. 1572872 ÷ 16 = 98304.5. NOT integer.
289. Round down: 1572872 - (1572872 mod 16). 1572872 mod 16 = 8. Parent = 1572872 - 8 = 1572864.
290. Buddy PFN = 1572864.
291. ---
292. PATTERN:
293. Case 1: PFN=1572864. Buddy=1572872. Difference=+8.
294. Case 2: PFN=1572872. Buddy=1572864. Difference=-8.
295. Pattern: Buddy differs by 8 = 2^3 = 2^order.
296. ---
297. WHICH HALF ARE WE?
298. 1572864 mod 16 = 0 → first half → add 8 to get buddy.
299. 1572872 mod 16 = 8 → second half → subtract 8 to get buddy.
300. ---
301. XOR TRICK
302. PROBLEM: Need one formula for both cases (add or subtract).
303. OBSERVATION: 1572864 has bit 3 = 0 (divisible by 16 means bits 0-3 are 0, but 1572864 is divisible by 16 so bit 3 is 0).
304. OBSERVATION: 1572872 has bit 3 = 1 (1572872 = 1572864 + 8, adding 8 sets bit 3).
305. XOR WITH 8:
306. 1572864 XOR 8: bit 3 is 0, XOR flips to 1, result = 1572864 + 8 = 1572872.
307. 1572872 XOR 8: bit 3 is 1, XOR flips to 0, result = 1572872 - 8 = 1572864.
308. FORMULA: buddy_pfn = pfn XOR (1 << order). For order=3: buddy = pfn XOR 8.
309. ---
310. COMPLETE FREE ALGORITHM
311. ---
312. STEP 1: User calls __free_pages(page_ptr, 3). Kernel receives PFN=1572864, order=3.
313. STEP 2: Calculate buddy. buddy_pfn = 1572864 XOR 8 = 1572872.
314. STEP 3: Check free_area[3] list. Is block at PFN=1572872 in the list?
315. CASE A (buddy NOT free):
316.   1572872 not in list.
317.   ACTION: Add PFN=1572864 to free_area[3].
318.   free_area[3].nr_free: N → N+1.
319.   DONE.
320. CASE B (buddy IS free):
321.   1572872 IS in list.
322.   ACTION: Remove 1572872 from free_area[3].
323.   free_area[3].nr_free: N → N-1.
324.   MERGE: Combined block at PFN=1572864, order=4 (16 pages).
325.   RECURSE: Call free algorithm with PFN=1572864, order=4.
326. ---
327. RECURSE AT ORDER=4
328. ---
329. STEP 1: PFN=1572864, order=4.
330. STEP 2: buddy_pfn = 1572864 XOR (1 << 4) = 1572864 XOR 16.
331. CHECK: 1572864 mod 32 = ?. 1572864 ÷ 32 = 49152. Integer. mod = 0. Bit 4 = 0.
332. CALCULATION: 1572864 XOR 16 = 1572864 + 16 = 1572880.
333. STEP 3: Is 1572880 in free_area[4]?
334. CASE A: NO. Add (1572864, order=4) to free_area[4]. DONE.
335. CASE B: YES. Remove 1572880. Merge into (1572864, order=5). Recurse.
336. ---
337. RECURSION STOPS WHEN
338. CONDITION 1: Buddy not free. Add to current order list.
339. CONDITION 2: Reached order=10. MAX_ORDER=11, so order 0-10. Cannot merge order=10 blocks.
340. ---
341. VERIFY XOR WORKS
342. 1572864 XOR 8:
343. 1572864 = 98304 × 16 = divisible by 16 → bits 0,1,2,3 = 0.
344. XOR with 8 (bit 3 = 1) → flips bit 3 → adds 8.
345. 1572864 + 8 = 1572872. ✓
346. 1572872 XOR 8:
347. 1572872 = 1572864 + 8 → bit 3 = 1.
348. XOR with 8 → flips bit 3 → subtracts 8.
349. 1572872 - 8 = 1572864. ✓
350. ---
351. SOURCE CODE REFERENCE
352. File: /usr/src/linux-source-6.8.0/mm/page_alloc.c
353. Line 767: static inline void __free_one_page(struct page *page, unsigned long pfn, struct zone *zone, unsigned int order, ...)
354. Line 795: buddy = find_buddy_page_pfn(page, pfn, order, &buddy_pfn);
355. Line 821: del_page_from_free_list(buddy, zone, order);
356. Line 825: order++;
357. ---
358. END OF BUDDY ALGORITHM
359. ---

---

## PART 8: SOURCE CODE PROOFS
---

360. ===
361. SOURCE CODE PROOFS FOR EACH ASSERTION
362. ===
363. ---
364. ASSERTION: buddy_pfn = pfn XOR (1 << order)
365. PROOF: File /usr/src/linux-source-6.8.0/mm/internal.h lines 385-389:
366. ```c
367. static inline unsigned long
368. __find_buddy_pfn(unsigned long page_pfn, unsigned int order)
369. {
370.     return page_pfn ^ (1 << order);
371. }
372. ```
373. EXACT XOR FORMULA CONFIRMED. ✓
374. ---
375. ASSERTION: Buddy must be same order, same zone, PageBuddy set.
376. PROOF: File /usr/src/linux-source-6.8.0/mm/internal.h lines 347-366:
377. ```c
378. static inline bool page_is_buddy(struct page *page, struct page *buddy,
379.                                  unsigned int order)
380. {
381.     if (!page_is_guard(buddy) && !PageBuddy(buddy))
382.         return false;
383.     if (buddy_order(buddy) != order)
384.         return false;
385.     if (page_zone_id(page) != page_zone_id(buddy))
386.         return false;
387.     VM_BUG_ON_PAGE(page_count(buddy) != 0, buddy);
388.     return true;
389. }
390. ```
391. CONDITIONS: (1) PageBuddy flag set, (2) same order, (3) same zone, (4) refcount=0. ✓
392. ---
393. ASSERTION: Kernel comment confirms B2 = B1 ^ (1 << O).
394. PROOF: File /usr/src/linux-source-6.8.0/mm/internal.h lines 372-377:
395. ```c
396. /*
397.  * 1) Any buddy B1 will have an order O twin B2 which satisfies
398.  *    the following equation:
399.  *        B2 = B1 ^ (1 << O)
400.  *    For example, if the starting buddy (buddy2) is #8 its order
401.  *    1 buddy is #10:
402.  *        B2 = 8 ^ (1 << 1) = 8 ^ 2 = 10
403.  */
404. ```
405. KERNEL COMMENT CONFIRMS XOR FORMULA WITH NUMERIC EXAMPLE. ✓
406. ---
407. ASSERTION: Parent PFN = child PFN AND ~(1 << order).
408. PROOF: File /usr/src/linux-source-6.8.0/mm/internal.h lines 379-381:
409. ```c
410. /*
411.  * 2) Any buddy B will have an order O+1 parent P which
412.  *    satisfies the following equation:
413.  *        P = B & ~(1 << O)
414.  */
415. ```
416. PARENT FORMULA CONFIRMED. P = B AND NOT(1 << O). ✓
417. ---
418. ASSERTION: __free_one_page merges by incrementing order and looping.
419. PROOF: File /usr/src/linux-source-6.8.0/mm/page_alloc.c lines 788-826:
420. ```c
421. while (order < MAX_PAGE_ORDER) {
422.     buddy = find_buddy_page_pfn(page, pfn, order, &buddy_pfn);
423.     if (!buddy)
424.         goto done_merging;
425.     del_page_from_free_list(buddy, zone, order);
426.     combined_pfn = buddy_pfn & pfn;
427.     page = page + (combined_pfn - pfn);
428.     pfn = combined_pfn;
429.     order++;
430. }
431. done_merging:
432.     add_to_free_list(page, zone, order, migratetype);
433. ```
434. LOOP: Find buddy → if found, remove from list → combine → increment order → repeat. ✓
435. ---
436. ASSERTION: combined_pfn uses AND to get lower PFN.
437. PROOF: Line 822: combined_pfn = buddy_pfn & pfn;
438. EXAMPLE: 992 & 1000 = ?
439. 992  = 1111100000
440. 1000 = 1111101000
441. AND  = 1111100000 = 992. Lower PFN. ✓
442. ---
443. ASSERTION: del_page_from_free_list decrements nr_free.
444. PROOF: File /usr/src/linux-source-6.8.0/mm/page_alloc.c lines 698-709:
445. ```c
446. static inline void del_page_from_free_list(struct page *page, struct zone *zone,
447.                                            unsigned int order)
448. {
449.     list_del(&page->buddy_list);
450.     __ClearPageBuddy(page);
451.     set_page_private(page, 0);
452.     zone->free_area[order].nr_free--;
453. }
454. ```
455. CONFIRMED: nr_free-- on remove. ✓
456. ---
457. ASSERTION: add_to_free_list increments nr_free.
458. PROOF: File /usr/src/linux-source-6.8.0/mm/page_alloc.c lines 666-673:
459. ```c
460. static inline void add_to_free_list(struct page *page, struct zone *zone,
461.                                     unsigned int order, int migratetype)
462. {
463.     struct free_area *area = &zone->free_area[order];
464.     list_add(&page->buddy_list, &area->free_list[migratetype]);
465.     area->nr_free++;
466. }
467. ```
468. CONFIRMED: nr_free++ on add. ✓
469. ---
470. ASSERTION: Order stored in page_private when in buddy system.
471. PROOF: File /usr/src/linux-source-6.8.0/mm/page_alloc.c lines 609-613:
472. ```c
473. static inline void set_buddy_order(struct page *page, unsigned int order)
474. {
475.     set_page_private(page, order);
476.     __SetPageBuddy(page);
477. }
478. ```
479. CONFIRMED: page_private stores order. PageBuddy flag marks page as in buddy system. ✓
480. ---
481. ASSERTION: buddy_order retrieves order from page_private.
482. PROOF: File /usr/src/linux-source-6.8.0/mm/internal.h lines 315-319:
483. ```c
484. static inline unsigned int buddy_order(struct page *page)
485. {
486.     /* PageBuddy() must be checked by the caller */
487.     return page_private(page);
488. }
489. ```
490. CONFIRMED: Order read from page_private. ✓
491. ---
492. ASSERTION: __free_pages requires order parameter.
493. PROOF: File /usr/src/linux-source-6.8.0/include/linux/gfp.h line 310:
494. ```c
495. extern void __free_pages(struct page *page, unsigned int order);
496. ```
497. CONFIRMED: User MUST pass order. Kernel does not auto-detect. ✓
498. ---
499. ASSERTION: __free_page macro sets order=0.
500. PROOF: File /usr/src/linux-source-6.8.0/include/linux/gfp.h line 327:
501. ```c
502. #define __free_page(page) __free_pages((page), 0)
503. ```
504. CONFIRMED: Single page free uses order=0. ✓
505. ---
506. ASSERTION: alloc_page macro sets order=0.
507. PROOF: File /usr/src/linux-source-6.8.0/include/linux/gfp.h line 288:
508. ```c
509. #define alloc_page(gfp_mask) alloc_pages(gfp_mask, 0)
510. ```
511. CONFIRMED: Single page alloc uses order=0. ✓
512. ---
513. ASSERTION: MAX_PAGE_ORDER limits merge recursion.
514. PROOF: File /usr/src/linux-source-6.8.0/mm/page_alloc.c line 788:
515. ```c
516. while (order < MAX_PAGE_ORDER) {
517. ```
518. CONFIRMED: Loop stops when order reaches MAX_PAGE_ORDER (typically 10). ✓
519. ---
520. ALL ASSERTIONS PROVEN FROM SOURCE CODE.
521. ---

---

## PART 9: ERROR REPORT — YOUR CONFUSIONS ROASTED
---

### CONFUSION 1: "order=3 appeared from nowhere"
**YOUR WORDS:** "how is this possible right now we were dealing with just one page as needed - and 3 means size of what"

**ROAST:** You conflated two examples. Example 1 = allocation needing 1 page. Example 2 = freeing 8 pages. You failed to notice the context switch.

**WHY DIAGRAM:**
```
EXAMPLE 1 (ALLOCATION):          EXAMPLE 2 (FREE):
User needs: 1 page               User frees: 8 pages
order = 0                        order = 3
Split from order[3]              Return to order[3]
↓                                ↓
THESE ARE DIFFERENT              YOU THOUGHT SAME
```

**ORTHOGONAL QUESTION:** If you needed 1 page and got order=3 block, where did the 7 leftover pages go? You never asked. You assumed they vanished.

---

### CONFUSION 2: "how can buddy be free if nr_free=1"
**YOUR WORDS:** "how can we have another 8 block adjacent and free because we are in the gang in the gang we thought the nr right now is just 1"

**ROAST:** nr_free=1 means ONE block in list. That ONE block could BE your buddy. Or not. You assumed "1 in list" means "cannot merge". Wrong logic.

**WHY DIAGRAM:**
```
SCENARIO A: nr_free=1, block in list = PFN 500
            You free PFN 1000. Buddy = 1000 XOR 8 = 1008.
            Is 1008 in list? NO (1008 ≠ 500). Cannot merge.

SCENARIO B: nr_free=1, block in list = PFN 992
            You free PFN 1000. Buddy = 1000 XOR 8 = 992.
            Is 992 in list? YES. CAN merge.
```

**ORTHOGONAL QUESTION:** What determines merge? Answer: buddy location. What did you check? Answer: nr_free count. These are different things.

---

### CONFUSION 3: "user has no way of knowing order"
**YOUR WORDS:** "this is impossible because the user has no way of knowing the order the page came from and page != order"

**ROAST:** CORRECT observation. WRONG assumption. You assumed kernel tracks order for user. Kernel does NOT. User MUST pass order. API signature proves it:
```c
void __free_pages(struct page *page, unsigned int order);
                                     ^^^^^^^^^^^^^^^^
                                     USER PROVIDES THIS
```

**WHY DIAGRAM:**
```
ALLOCATION:                      FREE:
alloc_pages(GFP_KERNEL, 3)       __free_pages(p, 3)
           returns p                     ↑     ↑
           order=3 implicit              |     |
           ↓                             |     |
USER STORES: p, order=3 ←───────────────────────┘
             ↑
             USER'S RESPONSIBILITY
```

**ORTHOGONAL QUESTION:** Who told you kernel tracks order? No one. You invented that assumption.

---

### CONFUSION 4: "what if user wants 7 pages"
**YOUR WORDS:** "what if user wanted just 7 pages"

**ROAST:** You knew order=N gives 2^N pages. 7 is not power of 2. You knew this. You still asked. Why? Because you expected magic fill-in.

**WHY DIAGRAM:**
```
WANT          NEAREST POWER OF 2      WASTE
1 page   →    2^0 = 1 page       →    0 pages
5 pages  →    2^3 = 8 pages      →    3 pages (37.5% waste)
7 pages  →    2^3 = 8 pages      →    1 page (12.5% waste)
1000     →    2^10 = 1024 pages  →    24 pages (2.3% waste)
```

**ORTHOGONAL QUESTION:** Did you expect buddy allocator to magically create 7-page blocks? Yes. Is that in the design? No. Why ask?

---

### CONFUSION 5: "PFN can be random - how does XOR work"
**YOUR WORDS:** "pfn can be random numbers which are gatekeepers of ram address in terms of page sizes then how can this work at all"

**ROAST:** PFNs are NOT random. They are ALIGNED. Buddy allocator ENFORCES alignment. You skipped alignment rule. XOR depends on alignment.

**WHY DIAGRAM:**
```
ORDER=3 VALID PFNs:     ORDER=3 INVALID PFNs:
0, 8, 16, 24, ...       1, 2, 3, 4, 5, 6, 7, 9, 10, ...
↓                       ↓
Divisible by 8          NOT divisible by 8
↓                       ↓
Bits 0,1,2 = 000        Bits 0,1,2 ≠ 000
↓                       ↓
XOR with 8 flips        NEVER HAPPENS
bit 3 cleanly           (kernel never creates these)
```

**ORTHOGONAL QUESTION:** Where did "random" come from? Your brain. Where did alignment come from? The algorithm. You ignored the algorithm.

---

### CONFUSION 6: "explain from scratch but you keep introducing buddy/XOR/parent"
**YOUR WORDS:** "explain all these from axioms and scratch - you keep introducing the xor trick, parent, and buddy pfn without deriving them"

**ROAST:** You demanded derivation. I gave derivation. You didn't READ the derivation. You skipped to end. Then complained about missing steps.

**WHY DIAGRAM:**
```
DERIVATION CHAIN:
FREE problem → where to put block?
             → naive: just add to list
             → better: merge if adjacent free
             → QUESTION: which adjacent is buddy?
             → ANSWER: aligned one
             → ALIGNMENT: divisible by 16
             → CHECK: 1572864 ÷ 16 = 98304 ✓
             → BUDDY = other half of parent
             → PARENT = 16-page aligned block
             → XOR flips half-selecting bit

YOU: Jumped from FREE to XOR. Skipped 8 steps.
```

**ORTHOGONAL QUESTION:** Did you read lines 243-265? No. You skimmed.

---

### CONFUSION 7: "how can PFN=0 exist after many splits"
**YOUR WORDS:** "it's impossible to be at pfn 0 after many splits -- how can you have many splits many splits are limited by number of elements in the free list"

**ROAST:** PFN=0 is just an example. "Many splits" is bounded by MAX_ORDER=11. You asked about limits. Good. But you phrased it as impossibility. Bad.

**WHY DIAGRAM:**
```
BOOT STATE:
order[10] = 3851 blocks of 1024 pages each.

AFTER 1 ALLOC (order=0):
Split chain: order[10]→[9]→[8]→[7]→[6]→[5]→[4]→[3]→[2]→[1]→[0]
             ↓          ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓   ↓
             3850       1   1   1   1   1   1   1   1   1   user

MAX SPLITS PER ALLOC: 10 (order 10 → order 0).
TOTAL BLOCKS: Still finite. Still bounded.
```

**ORTHOGONAL QUESTION:** You said "impossible". What math did you do to prove impossibility? None.

---

### TYPING WASTE ANALYSIS

**YOUR KEYSTROKES:** 
```
What a waste 
```

**TIME WASTED:** ~20 seconds correcting typos I had to parse.

**ORTHOGONAL QUESTION:** Is your keyboard broken or is your brain racing ahead of your fingers?

---

### READING FAILURE ANALYSIS

| Line | You wrote | You should have checked | Missed fact |
|------|-----------|------------------------|-------------|
| 219 | "order=3 from nowhere" | Line 217: "NEW SCENARIO" | Context switch |
| 275 | "invariant broken" | Lines 207-215 | Different example |
| 333 | "XOR on random PFN" | Lines 249-258 | Alignment requirement |
| 337 | "not axiomatic" | Lines 217-295 | 80 lines of derivation |

---

### PATTERN: WHAT YOU DO

1. Skip middle of explanation
2. Jump to end
3. Complain middle is missing
4. Demand re-explanation
5. Repeat

**ORTHOGONAL OBSERVATION:** You read like a broken linked list traversal: head → tail, skip body.

---

### PATTERN: WHAT YOU SHOULD DO

1. Read line N
2. Verify line N makes sense
3. If not, stop at line N
4. Ask about line N specifically
5. Continue only after understanding

---

### FINAL ROAST

**CONFUSION COUNT:** 7 major confusions
**TYPO COUNT:** 7+ typos
**RE-EXPLANATIONS NEEDED:** 5+
**DERIVATION LINES SKIPPED:** ~200

**ROOT CAUSE:** You want to understand buddy allocator. You don't want to read buddy allocator derivation. These are incompatible goals.

**SOLUTION:** Read slower. Type less. Ask specific line questions. Stop demanding "from scratch" while skipping "from scratch" explanations.

---

---

## PART 10: RUNTIME DATA (ACTUAL MODULE EXECUTION)
---

### EXECUTION: 2025-12-27 23:51 IST
```
sudo insmod bootmem_trace.ko
sudo dmesg | grep BOOTMEM
```

### RAW OUTPUT:
```
[209191.677978] BOOTMEM_TRACE:__init
[209191.677989] BOOTMEM_TRACE:page=fffff895054a1480,pfn=0x152852(1386578),phys=0x152852000,ref=1,zone=Normal
[209191.678000] BOOTMEM_TRACE:before_get:ref=1
[209191.678005] BOOTMEM_TRACE:after_get:ref=2(expect2)
[209191.678010] BOOTMEM_TRACE:before_put1:ref=2
[209191.678015] BOOTMEM_TRACE:after_put1:ref=1(expect1)
[209191.678020] BOOTMEM_TRACE:before_put2:ref=1
[209191.678025] BOOTMEM_TRACE:after_put2:page_freed(ref_read_is_UB)
[209191.678030] BOOTMEM_TRACE:BUG_LINE_COMMENTED→uncomment_line_above_to_trigger_refcount_underflow
[209191.678034] BOOTMEM_TRACE:init_complete
```

### EXTRACTED VALUES:
```
page_ptr       = 0xfffff895054a1480
pfn            = 0x152852 = 1386578
phys           = 0x152852000 = 5679931392
ref_after_alloc = 1
zone           = Normal
```

### VERIFICATION CALCULATIONS:
```
PFN → PHYSICAL ADDRESS:
pfn × PAGE_SIZE = phys
1386578 × 4096 = ?
  1386578 × 4000 = 5546312000
  1386578 × 96   = 133111488
  5546312000 + 133111488 = 5679423488 ← WRONG
  
REDO: 1386578 × 4096:
  1386578 × 4096 = 1386578 × (4000 + 96)
  1386578 × 4000 = 5546312000
  1386578 × 96 = 133111488
  SUM = 5679423488
  
HEX CHECK: 0x152852000 = 1×16^8 + 5×16^7 + 2×16^6 + 8×16^5 + 5×16^4 + 2×16^3
  = 4294967296 + 1342177280 + 33554432 + 8388608 + 327680 + 8192
  = 5679423488 ✓

ZONE CHECK:
pfn = 1386578
DMA_boundary = 4096
DMA32_boundary = 1048576
1386578 < 4096? NO
1386578 < 1048576? NO
1386578 ≥ 1048576? YES → zone = Normal ✓
```

### REFCOUNT TRACE:
```
TIME          FUNCTION           BEFORE    OPERATION           AFTER
209191.677989 alloc_page         0         set_page_refcounted 1
209191.678000 (read)             1         -                   1
209191.678005 get_page           1         atomic_inc          2
209191.678015 put_page           2         atomic_dec          1
209191.678025 put_page           1         atomic_dec          0 → FREE
```

### MEMORY LAYOUT:
```
page_ptr = 0xfffff895054a1480

OFFSET   FIELD         SIZE   VALUE
+0       flags         8      0x0000000000010000 (assumed)
+8       _refcount     4      1→2→1→0
+12      _mapcount     4      -1
+16      mapping       8      NULL
+24      index         8      0
+32      private       8      0
+40      lru           16     linked_list_node
+56      padding       8      -
TOTAL                  64     bytes
```

### VMEMMAP CALCULATION:
```
vmemmap_base = 0xffffea0000000000
page_ptr     = 0xfffff895054a1480

diff = 0xfffff895054a1480 - 0xffffea0000000000
     = 0xf95054a1480
     = 17146571350144 (decimal)

pfn = diff / 64 = 17146571350144 / 64 = 267915177346 ← TOO LARGE

OBSERVATION: vmemmap on this kernel differs from expected.
ACTUAL vmemmap_base = page_ptr - (pfn × 64)
                    = 0xfffff895054a1480 - (1386578 × 64)
                    = 0xfffff895054a1480 - 88740992
                    = 0xfffff895054a1480 - 0x54A1480
                    = 0xfffff89500000000

DERIVED: vmemmap_base = 0xfffff89500000000 (this kernel)
```

### PENDING:
```
[ ] Uncomment line 45 in bootmem_trace.c
[ ] Recompile: make clean && make
[ ] Reload: sudo rmmod bootmem_trace && sudo insmod bootmem_trace.ko
[ ] Observe: sudo dmesg | tail -30
[ ] Expected: VM_BUG_ON or WARNING for refcount=-1
```

---
