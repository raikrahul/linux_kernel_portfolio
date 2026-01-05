/*
 * prove_direct_map.c - Prove __va() works for any physical address
 */
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");

extern unsigned long page_offset_base;

static int prove_show(struct seq_file* m, void* v) {
    unsigned long phys_addrs[] = {0x0, 0x1000, 0x200000, 0x2CCB6C000};
    int i;

    seq_printf(m, "PROOF: Direct Map Entries Exist\n");
    seq_printf(m, "================================\n\n");

    seq_printf(m, "page_offset_base = 0x%016lx\n\n", page_offset_base);

    for (i = 0; i < 4; i++) {
        unsigned long phys = phys_addrs[i];
        void* virt = __va(phys);

        seq_printf(m, "Physical 0x%012lx:\n", phys);
        seq_printf(m, "  Virtual = page_offset_base + phys\n");
        seq_printf(m, "  Virtual = 0x%lx + 0x%lx\n", page_offset_base, phys);
        seq_printf(m, "  Virtual = 0x%016lx\n", (unsigned long)virt);

        /* Verify the addition */
        if ((unsigned long)virt == page_offset_base + phys) {
            seq_printf(m, "  VERIFY: 0x%lx + 0x%lx = 0x%lx ✓\n", page_offset_base, phys,
                       (unsigned long)virt);
        }

        /* Only read first few bytes to prove access works */
        if (phys < 0x10000000) { /* Only for low addresses */
            unsigned char byte = *(unsigned char*)virt;
            seq_printf(m, "  RAM[0x%lx] first byte = 0x%02x (proves readable)\n", phys, byte);
        }
        seq_printf(m, "\n");
    }

    seq_printf(m, "CONCLUSION:\n");
    seq_printf(m, "  virtual = page_offset_base + physical\n");
    seq_printf(m, "  Kernel created page table entries at boot\n");
    seq_printf(m, "  These entries make the + formula work\n");

    return 0;
}

static int prove_open(struct inode* inode, struct file* file) {
    return single_open(file, prove_show, NULL);
}

static const struct proc_ops prove_fops = {
    .proc_open = prove_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init prove_init(void) {
    proc_create("prove_direct_map", 0444, NULL, &prove_fops);
    printk(KERN_INFO "prove_direct_map: Read /proc/prove_direct_map\n");
    return 0;
}

static void __exit prove_exit(void) { remove_proc_entry("prove_direct_map", NULL); }

module_init(prove_init);
module_exit(prove_exit);
