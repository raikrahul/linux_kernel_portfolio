# BUDDY FRAGMENTATION EXERCISE
## Axiomatic Derivation | Linux Kernel 6.14.0-37-generic | x86_64

---

### MACHINE DATA (AXIOMS)
```
cat /proc/buddyinfo:
Node 0, zone   Normal   761   628   345   167   306   194   91   31   20   11   0
               order:    0     1     2     3     4     5    6    7    8    9   10

nr_free_pages = 184010
PAGE_SIZE = 4096
```

---

## PART 1: ORDER CALCULATIONS (DO BY HAND)

01. AXIOM: order=N means 2^N pages.
02. CALCULATE: order=0 → 2^0 = ___ pages.
03. CALCULATE: order=1 → 2^1 = ___ pages.
04. CALCULATE: order=2 → 2^2 = ___ pages.
05. CALCULATE: order=3 → 2^3 = ___ pages.
06. CALCULATE: order=10 → 2^10 = ___ pages.
07. AXIOM: 1 page = 4096 bytes.
08. CALCULATE: order=3 → ___ pages × 4096 = ___ bytes.
09. CALCULATE: order=10 → ___ pages × 4096 = ___ bytes = ___ MB.

---

## PART 2: BUDDYINFO CALCULATIONS (DO BY HAND)

10. AXIOM: Normal zone order0 count = 761 blocks.
11. CALCULATE: order0 total pages = 761 × 2^0 = 761 × ___ = ___ pages.
12. AXIOM: Normal zone order1 count = 628 blocks.
13. CALCULATE: order1 total pages = 628 × 2^1 = 628 × ___ = ___ pages.
14. AXIOM: Normal zone order3 count = 167 blocks.
15. CALCULATE: order3 total pages = 167 × 2^3 = 167 × ___ = ___ pages.
16. AXIOM: Normal zone order10 count = 0 blocks.
17. CALCULATE: order10 total pages = 0 × 2^10 = 0 × ___ = ___ pages.

---

## PART 3: FRAGMENTATION PROBLEM

18. PROBLEM: alloc_pages(GFP_KERNEL, 10) needs order=10 block.
19. CHECK: order10 blocks = 0. Is 0 ≥ 1? ___ (✓ or ✗)
20. IF ✗: Split from order=11. MAX_ORDER = 11. order=11 does not exist.
21. RESULT: alloc_pages returns ___ (page pointer or NULL).
22. PARADOX: nr_free_pages = 184010. order=10 needs ___ pages. 184010 > 1024? ___
23. WHY FAIL? 1024 ___ pages must be contiguous (adjacent/scattered). They are ___.

---

## PART 4: BUDDY XOR (DO BY HAND)

24. AXIOM: buddy_pfn = pfn XOR (1 << order).
25. CALCULATE: order=0, pfn=1000. buddy = 1000 XOR (1 << 0) = 1000 XOR ___ = ___.
26. CALCULATE: order=1, pfn=1000. buddy = 1000 XOR (1 << 1) = 1000 XOR ___ = ___.
27. CALCULATE: order=3, pfn=1000. buddy = 1000 XOR (1 << 3) = 1000 XOR ___ = ___.
28. VERIFY: pfn=1000 + buddy=1008 should form 16-page parent. 1000/16 = ___ remainder ___. Valid parent start? ___

---

## PART 5: TRACE ALLOC_PAGES(GFP_KERNEL, 3)

29. INPUT: order=3.
30. KERNEL: Check free_area[3].nr_free = 167.
31. KERNEL: 167 ≥ 1? ___.
32. ACTION: Take first block from list. nr_free: 167 → ___.
33. ACTION: set_page_refcounted → _refcount = ___.
34. OUTPUT: page pointer to ___ contiguous pages at PFN=X.

---

## PART 6: TRACE __FREE_PAGES(PAGE, 3)

35. INPUT: page at PFN=X, order=3.
36. KERNEL: buddy_pfn = X XOR 8.
37. KERNEL: Is buddy in free_area[3]? ___ (YES merge, NO add).
38. IF YES: Remove buddy. Combine into order=4. Recurse.
39. IF NO: Add page to free_area[3]. nr_free: N → N+1.

---

## PART 7: VERIFICATION COMMANDS

40. BEFORE insmod: cat /proc/buddyinfo | grep Normal → order3 = ___.
41. sudo insmod buddy_fragment.ko
42. AFTER insmod: cat /proc/buddyinfo | grep Normal → order3 = ___.
43. CHECK: Did order3 decrease by 1 then increase by 1? ___.
44. sudo dmesg | grep BUDDY_FRAG → pfn values.
45. sudo rmmod buddy_fragment

---

## PART 8: FAILURE PREDICTIONS

F1. User forgets to multiply by 4096. order=3 → 8 pages, NOT 8 bytes.
F2. User confuses order with page count. order=3 ≠ 3 pages. order=3 = 2^3 = 8 pages.
F3. User forgets XOR for buddy. buddy = pfn + 8 WRONG for second half.
F4. User forgets to pass order to __free_pages. __free_pages(page, 0) WRONG size.
F5. User expects merge always. Merge only if buddy is ALSO free.

---

## ANSWERS (FILL AFTER TRYING)

```
02. 1
03. 2
04. 4
05. 8
06. 1024
08. 8 × 4096 = 32768 bytes
09. 1024 × 4096 = 4194304 bytes = 4 MB
11. 761 × 1 = 761 pages
13. 628 × 2 = 1256 pages
15. 167 × 8 = 1336 pages
17. 0 × 1024 = 0 pages
19. ✗
21. NULL
22. 1024, YES
23. contiguous, scattered
25. 1, 1001
26. 2, 1002
27. 8, 1008
28. 62, 8, NO (1000 not 16-aligned)
31. ✓
32. 166
33. 1
34. 8
```

---
