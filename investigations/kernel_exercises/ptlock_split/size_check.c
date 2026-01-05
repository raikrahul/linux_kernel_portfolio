#include <linux/mm.h>
#include <linux/module.h>
#include <linux/spinlock.h>

MODULE_LICENSE("GPL");

static int __init sizes_init(void) {
  printk("DATA_SIZEOF_PAGE: %lu\n", sizeof(struct page));
  printk("DATA_SIZEOF_SPINLOCK: %lu\n", sizeof(spinlock_t));
  /* Accessing ptl directly is brittle due to unions/configs.
     We rely on sizeof(struct page) to check alignment/packing. */
  return 0;
}
static void __exit sizes_exit(void) {}
module_init(sizes_init);
module_exit(sizes_exit);
