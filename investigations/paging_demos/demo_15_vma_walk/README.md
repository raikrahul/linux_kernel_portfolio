```
═══════════════════════════════════════════════════════════════════════════════
DEMO 15: VMA WALK - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Each process has multiple VMAs
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: WHAT IS A VMA?
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ VMA = Virtual Memory Area                                                  │
│                                                                             │
│ Each VMA represents a contiguous region of virtual address space:          │
│                                                                             │
│ struct vm_area_struct {                                                    │
│     unsigned long vm_start;     // Start address (inclusive)               │
│     unsigned long vm_end;       // End address (exclusive)                 │
│     unsigned long vm_flags;     // Permissions (VM_READ, VM_WRITE, etc)   │
│     struct file *vm_file;       // Backing file (or NULL for anon)        │
│     pgoff_t vm_pgoff;           // Offset in file (in pages)              │
│     ...                                                                    │
│ };                                                                          │
│                                                                             │
│ EXAMPLE PROCESS LAYOUT:                                                    │
│ ┌────────────────────────────────────────────────────────────┐             │
│ │ 0x400000-0x401000  r-xp  /bin/cat     [.text]             │             │
│ │ 0x601000-0x602000  r--p  /bin/cat     [.rodata]           │             │
│ │ 0x602000-0x603000  rw-p  /bin/cat     [.data+.bss]        │             │
│ │ 0x7f...00-0x7f...10  r-xp  /lib/libc.so [libc text]       │             │
│ │ 0x7f...20-0x7f...30  rw-p  [heap]      (anonymous)        │             │
│ │ 0x7ffe...00-0x7fff0000  rw-p  [stack]  (anonymous)        │             │
│ └────────────────────────────────────────────────────────────┘             │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: vm_flags DECODING
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ Flag constants (from <linux/mm.h>):                                        │
│                                                                             │
│ VM_READ      = 0x00000001  (readable)                                      │
│ VM_WRITE     = 0x00000002  (writable)                                      │
│ VM_EXEC      = 0x00000004  (executable)                                    │
│ VM_SHARED    = 0x00000008  (shared mapping)                                │
│ VM_MAYREAD   = 0x00000010  (mprotect can make readable)                   │
│ VM_MAYWRITE  = 0x00000020  (mprotect can make writable)                   │
│ VM_MAYEXEC   = 0x00000040  (mprotect can make executable)                 │
│ VM_MAYSHARE  = 0x00000080  (can be shared)                                │
│ VM_GROWSDOWN = 0x00000100  (stack, grows downward)                        │
│ VM_GROWSUP   = 0x00000200  (grows upward)                                 │
│ VM_DONTCOPY  = 0x00020000  (don't copy on fork)                           │
│                                                                             │
│ Example: vm_flags = 0x00100073                                             │
│ 0x73 = 0111 0011:                                                          │
│   VM_READ (0x01) ✓                                                         │
│   VM_WRITE (0x02) ✓                                                        │
│   VM_EXEC (0x04) ✗                                                         │
│   VM_SHARED (0x08) ✗                                                       │
│   VM_MAYREAD (0x10) ✓                                                      │
│   VM_MAYWRITE (0x20) ✓                                                     │
│   VM_MAYEXEC (0x40) ✓                                                      │
│                                                                             │
│ → This is a private, readable, writable, non-executable region            │
│ → Likely heap or data section                                              │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: WALK VMAs OF CURRENT PROCESS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ CODE:                                                                       │
│ struct mm_struct *mm = current->mm;                                        │
│ struct vm_area_struct *vma;                                                │
│                                                                             │
│ mmap_read_lock(mm);  // Take read lock on mm                               │
│ for (vma = mm->mmap; vma; vma = vma->vm_next) {                           │
│     printk("VMA: 0x%lx-0x%lx flags=%lx file=%pK\n",                       │
│            vma->vm_start, vma->vm_end,                                     │
│            vma->vm_flags, vma->vm_file);                                   │
│ }                                                                           │
│ mmap_read_unlock(mm);                                                      │
│                                                                             │
│ NOTE: mm->mmap is head of linked list                                      │
│ NOTE: In newer kernels, VMAs are in maple tree instead                    │
│                                                                             │
│ EXPECTED OUTPUT for /proc/demo_vma reader:                                 │
│ VMA: 0x55a12340000-0x55a12341000 flags=0x875 file=..reader                │
│ VMA: 0x7f8abc100000-0x7f8abc300000 flags=0x1875 file=libc.so              │
│ VMA: 0x7ffc12300000-0x7ffc12400000 flags=0x100173 file=NULL (stack)       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: VMA SIZE CALCULATION
─────────────────────────────────────────────────────────────────────────────────

Given: vma->vm_start = 0x7FFE5E400000, vma->vm_end = 0x7FFE5E500000

┌─────────────────────────────────────────────────────────────────────────────┐
│ Size = vm_end - vm_start                                                   │
│      = 0x7FFE5E500000 - 0x7FFE5E400000                                    │
│      = 0x100000 bytes                                                      │
│      = 1,048,576 bytes                                                     │
│      = 1 MB                                                                 │
│                                                                             │
│ Pages in this VMA:                                                         │
│      = 0x100000 / 4096                                                     │
│      = 1,048,576 / 4096                                                    │
│      = 256 pages                                                            │
│                                                                             │
│ TRAP: vm_end is EXCLUSIVE!                                                 │
│   Valid addresses: 0x7FFE5E400000 - 0x7FFE5E4FFFFF                        │
│   0x7FFE5E500000 is NOT in this VMA                                       │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: VMA vs PTE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ VMA describes INTENT:                                                       │
│   "This region SHOULD be readable, writable, backed by file X"             │
│   Virtual construct, no direct hardware meaning                            │
│                                                                             │
│ PTE describes CURRENT STATE:                                               │
│   "This page IS mapped to physical 0x12345000, R/W=1, P=1"                │
│   Hardware uses this for translation                                       │
│                                                                             │
│ RELATIONSHIP:                                                               │
│   VMA: vm_flags = VM_READ | VM_WRITE                                      │
│   But page might be:                                                        │
│   - Not present (P=0): Not yet faulted in, or swapped                     │
│   - R/W=0: COW page, write will trigger fault                             │
│                                                                             │
│ Page fault handler checks:                                                 │
│   1. Is address in any VMA? (find_vma)                                    │
│   2. Is access type allowed? (vma->vm_flags)                              │
│   3. If allowed but PTE missing/wrong: fix it                             │
│   4. If not allowed: SIGSEGV                                              │
│                                                                             │
│ This is how lazy allocation works:                                         │
│   mmap() creates VMA but no PTEs                                          │
│   First access → page fault → allocate page → create PTE                  │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 2: 0x00100073 = read+write, no exec, private, growsdown (stack area)
Problem 4: Size = 1 MB = 256 pages

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Accessing mm without lock → race condition
F2. Confusing vm_flags with PTE flags → different meanings
F3. Including vm_end in range → off-by-one error
F4. current->mm is NULL for kernel threads → crash
F5. Not using VMA iterator on newer kernels → wrong API
```
