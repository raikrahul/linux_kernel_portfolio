/*
 * DEMO 16: SLAB INFO
 * ══════════════════
 *
 * AXIOMATIC DIAGNOSIS (7 Ws)
 * ──────────────────────────
 *
 * 1. WHAT:
 *    Input: Requested Object Size (e.g., 64 bytes).
 *    Action: Retrieve initialized object from "Cache".
 *    Output: Pointer to Object.
 *
 *    Computation:
 *    Page = 4096 bytes.
 *    Object = 64 bytes.
 *    Objects/Page = 4096 / 64 = 64 objects.
 *
 * 2. WHY:
 *    - To reduce "Internal Fragmentation".
 *    - Using 4KB page for 64-byte object = 98% waste.
 *    - To reduce "Initialization Overhead".
 *    - Constructor runs once per object lifetime, not per alloc/free.
 *
 * 3. WHERE:
 *    - Inside Kernel Pages (Linear Map).
 *    - Managed by `kmem_cache` struct.
 *
 * 4. WHO:
 *    - Slab Allocator (SLUB/SLAB/SLOB).
 *    - Kernel subsystems (`task_struct`, `inode`).
 *
 * 5. WHEN:
 *    - High-frequency allocation of fixed-size structures.
 *    - Filesystem activity (millions of inodes).
 *    - Network traffic (millions of packets/sk_buffs).
 *
 * 6. WITHOUT:
 *    - `kmalloc` would be slow.
 *    - Memory fragmentation would crash system in days.
 *
 * 7. WHICH:
 *    - Which Cache? Determined by Name or Size.
 *    - "kmalloc-64", "task_struct", etc.
 *
 * ════════════════════════════════
 * DISTINCT NUMERICAL PUZZLE
 * ════════════════════════════════
 * Scenario: Cookie Factory
 * - Dough Sheet (Page) = 4096 sq cm.
 * - Cookie (Object) = 64 sq cm.
 *
 * Process 1 (Buddy/Raw):
 * - Customer orders 1 cookie.
 * - Need 1 sheet. Cut 1 cookie. Throw away rest.
 * - Waste = 4032 sq cm.
 *
 * Process 2 (Slab):
 * - Pre-cut entire sheet into 64 cookies.
 * - Store them in "Ready Tray".
 * - Customer orders 1 cookie.
 * - Hand out #1. (0 sec cut time).
 * - Customer returns cookie (Free)? Put back in slot #1.
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("r");
MODULE_DESCRIPTION("Demo 16: Slab Info");

/* Demonstrate slab allocation */
static struct kmem_cache* demo_cache;

struct demo_object {
    unsigned long data[8]; /* 64 bytes */
};

static int demo_slab_show(struct seq_file* m, void* v) {
    struct demo_object* obj;

    seq_printf(m, "═══════════════════════════════════════════════════════════\n");
    seq_printf(m, "DEMO 16: SLAB ALLOCATOR\n");
    seq_printf(m, "═══════════════════════════════════════════════════════════\n\n");

    seq_printf(m, "SLAB CONCEPT:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Problem: malloc/free many same-size objects = fragmentation\n");
    seq_printf(m, "  Solution: pre-allocate chunks, maintain freelist\n");
    seq_printf(m, "  Each 'cache' handles objects of one size\n\n");

    seq_printf(m, "DEMO CACHE:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  Object size: %lu bytes\n", sizeof(struct demo_object));
    seq_printf(m, "  Cache name: demo_cache\n\n");

    /* Allocate from cache */
    obj = kmem_cache_alloc(demo_cache, GFP_KERNEL);
    if (obj) {
        obj->data[0] = 0xDEADBEEF;
        seq_printf(m, "  Allocated object at 0x%lx\n", (unsigned long)obj);
        seq_printf(m, "  obj->data[0] = 0x%lx\n", obj->data[0]);
        kmem_cache_free(demo_cache, obj);
        seq_printf(m, "  Freed object\n\n");
    } else {
        seq_printf(m, "  Allocation failed\n\n");
    }

    seq_printf(m, "COMMON KERNEL CACHES:\n");
    seq_printf(m, "────────────────────────────────────────────────────────\n");
    seq_printf(m, "  task_struct:     process descriptor\n");
    seq_printf(m, "  mm_struct:       memory descriptor\n");
    seq_printf(m, "  vm_area_struct:  VMA\n");
    seq_printf(m, "  dentry:          directory entry cache\n");
    seq_printf(m, "  inode:           inode cache\n");
    seq_printf(m, "  buffer_head:     block I/O\n\n");

    seq_printf(m, "SEE: cat /proc/slabinfo\n");

    return 0;
}

static int demo_slab_open(struct inode* inode, struct file* file) {
    return single_open(file, demo_slab_show, NULL);
}

static const struct proc_ops demo_slab_ops = {
    .proc_open = demo_slab_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init demo_slab_init(void) {
    demo_cache = kmem_cache_create("demo_cache", sizeof(struct demo_object), 0, 0, NULL);
    if (!demo_cache) {
        pr_err("demo_slab: failed to create cache\n");
        return -ENOMEM;
    }

    proc_create("demo_slab", 0444, NULL, &demo_slab_ops);
    pr_info("demo_slab: loaded\n");
    return 0;
}

static void __exit demo_slab_exit(void) {
    remove_proc_entry("demo_slab", NULL);
    if (demo_cache) kmem_cache_destroy(demo_cache);
}

module_init(demo_slab_init);
module_exit(demo_slab_exit);
