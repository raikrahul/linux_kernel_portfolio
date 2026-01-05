```
═══════════════════════════════════════════════════════════════════════════════
DEMO 16: SLAB ALLOCATOR - WORKSHEET
═══════════════════════════════════════════════════════════════════════════════
Machine: AMD Ryzen 5 4600H | Kernel=6.14.0
═══════════════════════════════════════════════════════════════════════════════

PROBLEM 1: WHY SLAB?
─────────────────────────────────────────────────────────────────────────────────

SCENARIO: Need to allocate 1000 objects of 64 bytes each

USING BUDDY ALLOCATOR:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Each 64-byte object needs order-0 allocation (4KB page)                    │
│ Waste per object = 4096 - 64 = 4032 bytes = 98.4% waste!                  │
│ Total waste = 4032 × 1000 = 4,032,000 bytes = 3.8 MB                      │
│                                                                             │
│ Alternative: Pack objects into pages                                       │
│ Objects per page = floor(4096 / 64) = 64 objects                          │
│ Pages needed = ceil(1000 / 64) = 16 pages = 64 KB                         │
│ Waste = 16 × 4096 - 1000 × 64 = 65536 - 64000 = 1536 bytes = 2.3%        │
│                                                                             │
│ SLAB does this packing automatically!                                      │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 2: SLAB STRUCTURE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│                           SLAB CACHE ("my_cache")                          │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐  │
│  │                         SLAB 1 (4KB page)                           │  │
│  │  ┌──────┬──────┬──────┬──────┬──────┬──────┬─────┐                 │  │
│  │  │ Obj0 │ Obj1 │ Obj2 │ Obj3 │ ...  │Obj63 │ ??? │                 │  │
│  │  │ 64B  │ 64B  │ 64B  │ 64B  │      │ 64B  │     │                 │  │
│  │  └──────┴──────┴──────┴──────┴──────┴──────┴─────┘                 │  │
│  │                                    64 objects per slab              │  │
│  └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐  │
│  │                         SLAB 2 (4KB page)                           │  │
│  │  (similar layout)                                                    │  │
│  └─────────────────────────────────────────────────────────────────────┘  │
│                                                                             │
│  Free list: Obj3→Obj10→Obj42→NULL (empty slots linked)                    │
└─────────────────────────────────────────────────────────────────────────────┘

OBJECTS PER SLAB CALCULATION:
┌─────────────────────────────────────────────────────────────────────────────┐
│ Object size = 64 bytes                                                      │
│ Page size = 4096 bytes                                                      │
│ Slab metadata ≈ small (SLUB uses page->objects)                            │
│ Objects per slab = floor(4096 / 64) = 64                                   │
│                                                                             │
│ With 128-byte objects:                                                      │
│ Objects per slab = floor(4096 / 128) = 32                                  │
│                                                                             │
│ With 500-byte objects:                                                      │
│ Objects per slab = floor(4096 / 500) = 8                                   │
│ Waste per slab = 4096 - 8 × 500 = 4096 - 4000 = 96 bytes = 2.3%           │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 3: API USAGE
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ CREATE CACHE:                                                               │
│ struct kmem_cache *cache = kmem_cache_create(                              │
│     "my_objects",    // name (seen in /proc/slabinfo)                      │
│     64,              // object size in bytes                                │
│     0,               // alignment (0 = natural)                             │
│     0,               // flags (SLAB_HWCACHE_ALIGN, etc)                    │
│     NULL             // constructor (called on each new object)            │
│ );                                                                          │
│                                                                             │
│ ALLOCATE OBJECT:                                                            │
│ void *obj = kmem_cache_alloc(cache, GFP_KERNEL);                           │
│                                                                             │
│ FREE OBJECT:                                                                │
│ kmem_cache_free(cache, obj);                                               │
│                                                                             │
│ DESTROY CACHE (after all objects freed):                                   │
│ kmem_cache_destroy(cache);                                                 │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 4: TIMING ANALYSIS
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ SLAB ALLOC (hot path, object on CPU cache freelist):                       │
│   1. Disable preemption (~10 cycles)                                       │
│   2. Get per-CPU cache pointer (~3 cycles)                                 │
│   3. Pop object from freelist (~5 cycles)                                  │
│   4. Enable preemption (~10 cycles)                                        │
│   Total: ~30 cycles ≈ 10 ns @ 3 GHz                                       │
│                                                                             │
│ BUDDY ALLOC:                                                                │
│   1. Lock zone (~50 cycles with spinlock)                                  │
│   2. Search free lists (~100 cycles)                                       │
│   3. Split blocks if needed (~50 cycles per split)                         │
│   4. Update page flags (~20 cycles)                                        │
│   5. Unlock zone (~50 cycles)                                              │
│   Total: ~300+ cycles ≈ 100 ns @ 3 GHz                                    │
│                                                                             │
│ SPEEDUP: Slab is ~10× faster for small objects                            │
└─────────────────────────────────────────────────────────────────────────────┘

─────────────────────────────────────────────────────────────────────────────────
PROBLEM 5: COMMON CACHES
─────────────────────────────────────────────────────────────────────────────────

┌─────────────────────────────────────────────────────────────────────────────┐
│ View with: cat /proc/slabinfo | head -20                                   │
│                                                                             │
│ Example output:                                                             │
│ # name            <active_objs> <num_objs> <objsize>                       │
│ task_struct            1234       1500       4192                          │
│ file                  12000      15000        256                          │
│ dentry                50000      60000        192                          │
│ inode_cache           30000      35000        600                          │
│ kmalloc-64            10000      12000         64                          │
│ kmalloc-128            5000       6000        128                          │
│                                                                             │
│ kmalloc-* are generic caches used by kmalloc(size, GFP_KERNEL)            │
└─────────────────────────────────────────────────────────────────────────────┘

═══════════════════════════════════════════════════════════════════════════════
ANSWERS
═══════════════════════════════════════════════════════════════════════════════

Problem 1: Buddy wastes 98.4%, Slab wastes 2.3%
Problem 2: 64 objects per slab for 64-byte objects
Problem 4: Slab ~30 cycles, Buddy ~300 cycles (10× faster)

═══════════════════════════════════════════════════════════════════════════════
FAILURE PREDICTIONS
═══════════════════════════════════════════════════════════════════════════════

F1. Object size mismatch → memory corruption
F2. Double free → slab corruption, kernel crash
F3. Forgetting kmem_cache_destroy → memory leak
F4. Using wrong cache for free → corruption
F5. Object larger than slab size → allocation fails
```
