# MAPLE TREE: LINUX KERNEL VMA LOOKUP

**Target**: Kernel 6.1+ (your machine: 6.14.0-37-generic)
**Prerequisites**: Counting, binary, pointers, RAM concept
**Format**: Axiomatic derivation + numerical examples + exercises

---

## SECTION 1: THE PROBLEM

### 1.1 AXIOMS

```
A1. Process = running program.
A2. Process uses virtual addresses (VA) like 0x78d7ce727000.
A3. VA range [start, end) = Virtual Memory Area (VMA).
A4. Process can have many VMAs (code, stack, heap, mmap regions).
A5. Page fault = CPU tried to access VA with no physical page.
A6. Kernel must find which VMA contains the faulting VA.
```

### 1.2 NUMERICAL EXAMPLE

```
Process has 3 VMAs:
  VMA_A: [0x1000, 0x2000)  code
  VMA_B: [0x3000, 0x5000)  heap
  VMA_C: [0x7000, 0x8000)  stack

Page fault at VA = 0x3500.
Question: Which VMA contains 0x3500?
Answer: VMA_B (0x3000 <= 0x3500 < 0x5000) ✓
```

### 1.3 EXERCISE 1 (Do by hand)

```
VMAs: A=[0x10000, 0x20000), B=[0x30000, 0x40000), C=[0x50000, 0x60000)
Faulting VA = 0x35000.
Which VMA? _______
Check: 0x35000 >= 0x30000? ___ 0x35000 < 0x40000? ___
```

---

## SECTION 2: DATA STRUCTURES

### 2.1 NAIVE APPROACH: LINKED LIST

```
SOURCE: Old Linux kernels used linked list via vm_next/vm_prev.

struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    struct vm_area_struct *vm_next;  // REMOVED in kernel 6.1
};

ALGORITHM: Start at head, check each VMA, follow vm_next.
COMPLEXITY: O(N) - with 1000 VMAs, 1000 checks worst case.
```

### 2.2 BETTER APPROACH: RB-TREE (Kernel < 6.1)

```
SOURCE: include/linux/rbtree.h

struct rb_node {
    unsigned long __rb_parent_color;  // 8 bytes
    struct rb_node *rb_right;         // 8 bytes
    struct rb_node *rb_left;          // 8 bytes
};

EMBEDDED in vm_area_struct:
struct vm_area_struct {
    unsigned long vm_start;
    unsigned long vm_end;
    struct rb_node vm_rb;  // embedded, not pointer
};

ALGORITHM: Binary search tree, left/right based on vm_start.
COMPLEXITY: O(log₂ N) - with 1000 VMAs, log₂(1000) ≈ 10 checks.
```

### 2.3 CURRENT APPROACH: MAPLE TREE (Kernel 6.1+)

```
SOURCE: include/linux/maple_tree.h

struct maple_tree {
    spinlock_t ma_lock;
    unsigned int ma_flags;
    void __rcu *ma_root;   // Pointer to root node
};

struct mm_struct {
    struct maple_tree mm_mt;  // VMA tree (SOURCE: mm_types.h:781)
};

ALGORITHM: 16-way branching tree with pivots.
COMPLEXITY: O(log₁₆ N) - with 1000 VMAs, log₁₆(1000) ≈ 3 checks.
```

### 2.4 WHY ma_lock EXISTS (RACE CONDITION)

```
A1. CPU_0, CPU_1, ... CPU_N = multiple processors
A2. RAM = shared between all CPUs
A3. mm->mm_mt.ma_root = address in shared RAM

WITHOUT LOCK (race condition):
  t=0: CPU_0 reads ma_root → 0xffff888200000010
  t=1: CPU_1 writes ma_root → 0xffff888300000010 (mmap adds VMA)
  t=2: CPU_0 reads RAM[0xffff888200000000] → FREED/GARBAGE ✗

PROBLEM:
  CPU_0_read(ma_root) || CPU_1_write(ma_root) → race condition
  CPU_0 uses stale pointer → crash ✗

WITH LOCK (sequential):
  t=0: CPU_0 acquire(ma_lock) → success
  t=1: CPU_1 acquire(ma_lock) → BLOCKED (spins)
  t=2: CPU_0 reads ma_root → 0xffff888200000010
  t=3: CPU_0 reads RAM[node] → valid data ✓
  t=4: CPU_0 release(ma_lock)
  t=5: CPU_1 acquire(ma_lock) → success
  t=6: CPU_1 writes ma_root → 0xffff888300000010
  t=7: CPU_1 release(ma_lock)

∴ lock → sequential access → no race → no crash ✓

spinlock = busy-wait loop:
  acquire: while (lock != 0) {} ; lock = 1
  release: lock = 0

RCU (__rcu annotation):
  readers: no lock needed (read old or new, both valid)
  writers: wait for all readers to finish before freeing old
  ∴ read-heavy workloads → RCU faster than spinlock
```

---

## SECTION 3: MAPLE NODE LAYOUT

### 3.1 NODE STRUCTURE (ANNOTATED)

```
SOURCE: include/linux/maple_tree.h:103-113

struct maple_range_64 {

    struct maple_pnode *parent;
    // WHAT: 8-byte pointer to parent node
    // WHERE: offset 0 → RAM[node_base + 0] to RAM[node_base + 7]
    // WHY: tree traversal upward (child → parent)
    // WHO: set by mas_store() when node inserted
    // WHEN: node creation time
    // WITHOUT: orphan node, cannot find root
    // EXAMPLE: node at 0xffff888200000000, parent = 0xffff888100000000
    //   RAM[0xffff888200000000] = 00 00 00 00 01 88 ff ff (little-endian)
    // EDGE: root node → parent = NULL = 0x0000000000000000
    // LARGE: 1000 VMAs = 3 levels = 1 root (parent=NULL) + 16 internal (parent=root) + 256 leafs (parent=internal)

    unsigned long pivot[MAPLE_RANGE64_SLOTS - 1];
    // WHAT: 15 × 8 = 120 bytes array of boundary values
    // WHERE: offset 8 → RAM[node_base + 8] to RAM[node_base + 127]
    // WHY: pivot[i] = LAST address covered by slot[i]
    // WHO: set by mas_store() based on VMA vm_end - 1
    // WHEN: VMA insertion time
    // WITHOUT: no way to know which slot matches query address
    // CALCULATION: MAPLE_RANGE64_SLOTS = 16 → pivot count = 16 - 1 = 15
    //   15 pivots × 8 bytes = 120 bytes
    //   pivot[0] at offset 8 + 0×8 = 8
    //   pivot[7] at offset 8 + 7×8 = 8 + 56 = 64 → cache line 1 boundary!
    //   pivot[14] at offset 8 + 14×8 = 8 + 112 = 120
    // EXAMPLE 3 VMAs:
    //   VMA_A: [0x1000, 0x2000) → pivot[0] = 0x1fff (vm_end - 1 = 0x2000 - 1)
    //   VMA_B: [0x3000, 0x5000) → pivot[1] = 0x4fff
    //   VMA_C: [0x7000, 0x8000) → pivot[2] = 0x7fff
    //   pivot[3..14] = 0 (unused)
    // MIDDLE EXAMPLE: Query 0x4000
    //   0x4000 <= pivot[0] (0x1fff)? NO (0x4000 > 0x1fff)
    //   0x4000 <= pivot[1] (0x4fff)? YES → slot[1] = VMA_B ✓
    // EDGE N=1: pivot[0] = vm_end - 1, pivot[1..14] = 0
    // EDGE N=16: all 15 pivots used, can still hold 16 VMAs (slot[15] covers > pivot[14])
    // LARGE N=256: 16 leaf nodes, each with 15 pivots = 240 pivots total for boundaries
    // CACHE: pivot[0..6] in cache line 0 (offset 8-63), pivot[7..13] in cache line 1 (offset 64-127)

    union {
        void __rcu *slot[MAPLE_RANGE64_SLOTS];
        // WHAT: 16 × 8 = 128 bytes array of pointers
        // WHERE: offset 128 → RAM[node_base + 128] to RAM[node_base + 255]
        // WHY: slot[i] points to VMA (leaf) or child node (internal)
        // WHO: set by mas_store() with VMA pointer or child node pointer
        // WHEN: VMA insertion or tree split
        // WITHOUT: no data, tree is useless
        // WHICH: slot[i] covers addresses where: pivot[i-1] < addr <= pivot[i]
        //   slot[0] covers: 0 <= addr <= pivot[0]
        //   slot[1] covers: pivot[0] < addr <= pivot[1]
        //   slot[15] covers: pivot[14] < addr <= ULONG_MAX
        // CALCULATION:
        //   slot[0] at offset 8 + 15×8 + 0×8 = 8 + 120 + 0 = 128
        //   slot[7] at offset 128 + 7×8 = 128 + 56 = 184 → cache line 2
        //   slot[15] at offset 128 + 15×8 = 128 + 120 = 248
        // EXAMPLE (leaf with 3 VMAs):
        //   slot[0] = 0xffff8881abcd0000 → VMA_A
        //   slot[1] = 0xffff8881abcd0200 → VMA_B (VMA struct ~512 bytes apart)
        //   slot[2] = 0xffff8881abcd0400 → VMA_C
        //   slot[3..15] = NULL
        // EXAMPLE (internal with 3 children):
        //   slot[0] = 0xffff888300000010 → LEAF_A (encoded pointer with type bits!)
        //   slot[1] = 0xffff888300000110 → LEAF_B (256 bytes apart = aligned)
        //   slot[2] = 0xffff888300000210 → LEAF_C
        // MEMORY for slot[0]:
        //   RAM[0xffff888200000080] = 00 00 cd ab 81 88 ff ff (VMA at 0xffff8881abcd0000)
        // CACHE: slot[0..7] in cache line 2 (offset 128-191), slot[8..15] in cache line 3 (offset 192-255)
    };

};  // TOTAL: 8 + 120 + 128 = 256 bytes = 0x100 bytes = 4 cache lines (64 bytes each)

// FULL MEMORY LAYOUT AT NODE 0xffff888200000000:
// +--------+--------+-------------------------------------------+
// | OFFSET | SIZE   | CONTENT                                   |
// +--------+--------+-------------------------------------------+
// | 0x000  | 8      | parent = 0x0000000000000000 (root)        |
// | 0x008  | 8      | pivot[0] = 0x78d7ce727fff                 |
// | 0x010  | 8      | pivot[1] = 0x0 (unused)                   |
// | ...    | ...    | pivot[2..14] = 0                          |
// | 0x078  | 8      | pivot[14] = 0x0                           |
// | 0x080  | 8      | slot[0] = 0xffff8881abcd0000 (VMA ptr)    |
// | 0x088  | 8      | slot[1] = NULL                            |
// | ...    | ...    | slot[2..15] = NULL                        |
// | 0x0F8  | 8      | slot[15] = NULL                           |
// +--------+--------+-------------------------------------------+
// TOTAL: 0x100 = 256 bytes

MAPLE_RANGE64_SLOTS = 16 (SOURCE: maple_tree.h:30)
// WHY 16? 16 slots × 8 bytes = 128 bytes. 
// 128 + 120 (pivots) + 8 (parent) = 256 bytes = 4 cache lines.
// 16-way branching → log₁₆(1000) = 2.5 ≈ 3 levels for 1000 VMAs.
// Compare: binary tree log₂(1000) = 10 levels. Maple is 3.3× shallower.
```

### 3.2 MEMORY LAYOUT DIAGRAM

```
+---maple_range_64 (256 bytes)---+
| OFFSET 0:   parent (8 bytes)  |
| OFFSET 8:   pivot[0] (8 bytes)|  ← boundary for slot[0]
| OFFSET 16:  pivot[1] (8 bytes)|  ← boundary for slot[1]
| OFFSET 24:  pivot[2] (8 bytes)|
| ...                           |
| OFFSET 112: pivot[13]         |
| OFFSET 120: pivot[14]         |
| OFFSET 128: slot[0] (8 bytes) |  ← VMA pointer or child node
| OFFSET 136: slot[1] (8 bytes) |
| ...                           |
| OFFSET 248: slot[15](8 bytes) |
+-------------------------------+
```

### 3.3 OFFSET CALCULATION

```
DERIVE pivot[i] offset:
  pivot[0] offset = parent_size + i × 8 = 8 + 0 × 8 = 8
  pivot[5] offset = 8 + 5 × 8 = 48

DERIVE slot[i] offset:
  slot[i] offset = parent_size + pivots_size + i × 8
                 = 8 + 15×8 + i×8
                 = 8 + 120 + i×8
                 = 128 + i×8

  slot[0] offset = 128 + 0×8 = 128
  slot[7] offset = 128 + 7×8 = 184
```

### 3.4 EXERCISE 2 (Calculate offsets)

```
Q1: Offset of pivot[10]? = 8 + 10×8 = ____
Q2: Offset of slot[3]?  = 128 + 3×8 = ____
Q3: If node base = 0xffff888200000000, address of pivot[0] = ____
Q4: If node base = 0xffff888200000000, address of slot[0] = ____
```

---

## SECTION 4: POINTER ENCODING

### 4.1 TYPE ENCODING IN ma_root

```
SOURCE: lib/maple_tree.c:223

AXIOM: Maple nodes are 256-byte aligned.
DERIVATION: 256 = 0x100. Low 8 bits of address = 0.
TRICK: Use low bits to encode metadata.

MAPLE_NODE_TYPE_SHIFT = 3 (SOURCE: maple_tree.h:180)
MAPLE_NODE_TYPE_MASK = 0x0F (SOURCE: maple_tree.h:179)

enum maple_type {      // SOURCE: maple_tree.h:144-149
    maple_dense = 0,
    maple_leaf_64 = 1,
    maple_range_64 = 2,  ← VMA trees use this
    maple_arange_64 = 3,
};
```

### 4.2 ENCODING FORMULA

```
ma_root = node_address | (type << SHIFT)
        = node_address | (type << 3)

EXAMPLE:
  node_address = 0xffff888200000000
  type = 2 (maple_range_64)
  encoded_type = 2 << 3 = 16 = 0x10
  ma_root = 0xffff888200000000 | 0x10 = 0xffff888200000010
```

### 4.3 DECODING FORMULA

```
node_address = ma_root & 0xFFFFFFFFFFFFFF00  (clear low 8 bits)
type = (ma_root >> 3) & 0xF                   (extract bits 3-6)

EXAMPLE:
  ma_root = 0xffff888200000010
  node_address = 0xffff888200000010 & 0xFFFFFFFFFFFFFF00 = 0xffff888200000000
  type = (0x10 >> 3) & 0xF = 2 & 0xF = 2 (maple_range_64) ✓
```

### 4.4 EXERCISE 3 (Encode/Decode)

```
Q1: Encode node=0xffff888300000000, type=2: ma_root = ____
Q2: Decode ma_root=0xffff888400000018: node_address = ____, type = ____
Q3: Why does 0x18 mean type=3? Show calculation: ____
```

---

## SECTION 5: LOOKUP ALGORITHM

### 5.1 PIVOT MEANING

```
AXIOM: pivot[i] = LAST address covered by slot[i].
RULE: If query_addr <= pivot[i], answer is in slot[i].

EXAMPLE: 3 VMAs
  VMA_A: [0x1000, 0x2000)  → pivot[0] = 0x1fff (vm_end - 1)
  VMA_B: [0x3000, 0x5000)  → pivot[1] = 0x4fff
  VMA_C: [0x7000, 0x8000)  → pivot[2] = 0x7fff

Query addr = 0x3500:
  Is 0x3500 <= pivot[0] (0x1fff)? NO
  Is 0x3500 <= pivot[1] (0x4fff)? YES → return slot[1] = VMA_B ✓
```

### 5.2 STEP-BY-STEP ALGORITHM

```
INPUT: faulting_addr = 0x78d7ce727100

STEP 1: Read ma_root from mm->mm_mt
        Value = 0xffff888200000010

STEP 2: Decode
        node = 0xffff888200000000
        type = 2

STEP 3: Read pivot[0] from RAM[node + 8]
        Value = 0x78d7ce727fff

STEP 4: Compare
        Is 0x78d7ce727100 <= 0x78d7ce727fff? YES

STEP 5: Read slot[0] from RAM[node + 128]
        Value = 0xffff8881abcd0000 (VMA pointer)

STEP 6: Verify VMA range
        vm_start = 0x78d7ce727000
        vm_end = 0x78d7ce728000
        0x78d7ce727000 <= 0x78d7ce727100 < 0x78d7ce728000? YES ✓

OUTPUT: VMA found at 0xffff8881abcd0000
```

### 5.3 EXERCISE 4 (Trace lookup)

```
Given:
  ma_root = 0xffff888300000010
  pivot[0] = 0x5fff, pivot[1] = 0x9fff, pivot[2] = 0xffff
  slot[0] = VMA_A, slot[1] = VMA_B, slot[2] = VMA_C
  faulting_addr = 0x7500

Trace:
  Step 1: node_address = 0xffff888300000010 & ~0xFF = ____
  Step 2: type = (0x10 >> 3) & 0xF = ____
  Step 3: Is 0x7500 <= 0x5fff? ____
  Step 4: Is 0x7500 <= 0x9fff? ____
  Step 5: Answer slot index = ____
  Step 6: VMA = ____
```

---

## SECTION 6: TREE GROWTH

### 6.1 SINGLE NODE (1-16 VMAs)

```
mm->mm_mt.ma_root → LEAF_NODE
                      slot[0] → VMA_A
                      slot[1] → VMA_B
                      ...
                      slot[15] → VMA_P
```

### 6.2 TWO LEVELS (17-256 VMAs)

```
mm->mm_mt.ma_root → INTERNAL_NODE
                      slot[0] → LEAF_A (VMAs 0-15)
                      slot[1] → LEAF_B (VMAs 16-31)
                      ...
                      slot[15] → LEAF_P (VMAs 240-255)
```

### 6.3 THREE LEVELS (257-4096 VMAs)

```
mm->mm_mt.ma_root → INTERNAL_1
                      slot[0] → INTERNAL_1A
                                  slot[0] → LEAF (VMAs 0-15)
                                  ...
                      slot[1] → INTERNAL_1B
                                  slot[0] → LEAF (VMAs 256-271)
                                  ...
```

### 6.4 CAPACITY FORMULA

```
Level 1: 16 VMAs
Level 2: 16 × 16 = 256 VMAs
Level 3: 16 × 16 × 16 = 4096 VMAs
Level d: 16^d VMAs

INVERSE: d levels needed for N VMAs: d = ⌈log₁₆(N)⌉
```

---

## SECTION 7: PERFORMANCE COMPARISON

### 7.1 YOUR MACHINE DATA

```
CPU: AMD Ryzen 5 4600H (SOURCE: /proc/cpuinfo)
L1d cache: 192 KiB (SOURCE: lscpu)
Cache line: 64 bytes (SOURCE: getconf LEVEL1_DCACHE_LINESIZE)
TLB: 3072 × 4K pages (SOURCE: /proc/cpuinfo)
```

### 7.2 CACHE ANALYSIS

```
Maple node = 256 bytes = 4 cache lines (256/64)
RB-tree node = ~40 bytes embedded in VMA (~200 bytes) = 4 cache lines

For 1000 VMAs:
  RB-tree: ~10 levels × 4 cache lines = 40 cache line fetches
  Maple:   ~3 levels × 4 cache lines = 12 cache line fetches

SPEEDUP: 40/12 = 3.3× fewer cache misses
```

### 7.3 COMPARISON TABLE

```
| Metric         | Linked List | RB-Tree    | Maple Tree |
|----------------|-------------|------------|------------|
| Complexity     | O(N)        | O(log₂ N)  | O(log₁₆ N) |
| 1000 VMAs      | 1000 ops    | 10 ops     | 3 ops      |
| Branching      | 1           | 2          | 16         |
| Node size      | N/A         | ~40 bytes  | 256 bytes  |
| Cache lines    | N/A         | ~40/lookup | ~12/lookup |
| RCU-safe       | No          | No         | Yes        |
```

---

## SECTION 8: CODE REFERENCES

### 8.1 KEY SOURCE FILES

```
include/linux/maple_tree.h     - struct definitions, macros
lib/maple_tree.c               - implementation
include/linux/mm_types.h:781   - mm_struct.mm_mt definition
include/linux/mm_types.h:649   - vm_area_struct definition
mm/memory.c:5712               - lock_vma_under_rcu uses mas_walk
kernel/fork.c:1260             - mm_init initializes mm_mt
```

### 8.2 KEY FUNCTIONS

```
mt_init_flags()     - Initialize maple tree (ma_root = NULL)
mas_walk()          - Lookup entry by index
mas_store_prealloc()- Insert entry
mte_node_type()     - Decode node type from pointer
```

---

## SECTION 9: EXERCISES ANSWERS

### Exercise 1

```
VMA_B contains 0x35000.
Check: 0x35000 >= 0x30000? YES. 0x35000 < 0x40000? YES.
```

### Exercise 2

```
Q1: 8 + 10×8 = 88
Q2: 128 + 3×8 = 152
Q3: 0xffff888200000000 + 8 = 0xffff888200000008
Q4: 0xffff888200000000 + 128 = 0xffff888200000080
```

### Exercise 3

```
Q1: 0xffff888300000000 | (2 << 3) = 0xffff888300000010
Q2: node = 0xffff888400000000, type = (0x18 >> 3) & 0xF = 3
Q3: 0x18 = 24 in decimal. 24 >> 3 = 3. 3 & 0xF = 3.
```

### Exercise 4

```
Step 1: 0xffff888300000000
Step 2: 2
Step 3: NO (0x7500 > 0x5fff)
Step 4: YES (0x7500 <= 0x9fff)
Step 5: 1
Step 6: VMA_B
```

---

## SECTION 10: SUMMARY

```
1. Maple Tree replaced RB-tree in kernel 6.1 for VMA management.
2. Each node is 256 bytes with 15 pivots and 16 slots.
3. Pivots define boundaries. slot[i] covers addresses <= pivot[i].
4. Type is encoded in low bits of pointer (shift 3, mask 0xF).
5. Lookup: compare address to pivots, follow matching slot.
6. Performance: 3× fewer cache misses than RB-tree for 1000 VMAs.
```

---

**Author**: Generated for Linux Kernel Memory Management Study
**Machine**: AMD Ryzen 5 4600H, Kernel 6.14.0-37-generic
**Sources**: /home/r/Desktop/learn_kernel/source/
