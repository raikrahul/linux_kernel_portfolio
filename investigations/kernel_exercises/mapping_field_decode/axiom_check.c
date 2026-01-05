#include <linux/fs.h> // struct address_space
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/page-flags.h> // PAGE_MAPPING_ANON
#include <linux/rmap.h>       // struct anon_vma

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Primate");
MODULE_DESCRIPTION("Axiom checker for mapping alignment");

static int __init axiom_init(void) {
  printk(KERN_INFO "AXIOM CHECK: Loading module...\n");

  // 1. Check Alignment of struct address_space
  printk(KERN_INFO "AXIOM 1: struct address_space alignment = %lu bytes\n",
         __alignof__(struct address_space));

  // 2. Check Alignment of struct anon_vma
  printk(KERN_INFO "AXIOM 2: struct anon_vma alignment = %lu bytes\n",
         __alignof__(struct anon_vma));

  // 3. Check Flag Values
  printk(KERN_INFO "AXIOM 3: PAGE_MAPPING_ANON = 0x%x\n", PAGE_MAPPING_ANON);
  printk(KERN_INFO "AXIOM 4: PAGE_MAPPING_MOVABLE = 0x%x\n",
         PAGE_MAPPING_MOVABLE);
  printk(KERN_INFO "AXIOM 5: PAGE_MAPPING_KSM = 0x%x\n", PAGE_MAPPING_KSM);

  // 4. Manual Verification
  if (__alignof__(struct address_space) < 4) {
    printk(KERN_ERR "FAILURE: Alignment too small for 2 bits!\n");
  } else {
    printk(KERN_INFO "SUCCESS: Alignment supports 2 low-bit flags.\n");
  }

  if (__alignof__(struct anon_vma) < 4) {
    printk(KERN_ERR "FAILURE: Alignment too small for 2 bits!\n");
  } else {
    printk(KERN_INFO "SUCCESS: Anon alignment OK.\n");
  }

  return 0;
}

static void __exit axiom_exit(void) {
  printk(KERN_INFO "AXIOM CHECK: Unloading.\n");
}

module_init(axiom_init);
module_exit(axiom_exit);
