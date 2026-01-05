#include <linux/init.h>(void)word;
#include <linux/kernel.h>
#include <linux/module.h>

// Define MANY global variables (uninitialized -> BSS)
static volatile unsigned long global_var_1;
static volatile unsigned long global_var_2;
static volatile unsigned long global_var_3;
static volatile unsigned long global_var_4;
static volatile unsigned long global_var_5;

// Define one initialized global (DATA section) to show contrast
static volatile unsigned long init_var_1 = 0xCAFEBABE;

static int __init reloc_test_init(void) {
    // Access them all to FORCE generated instructions
    // Each access will need a relocation entry
    global_var_1 = 0x11111111;
    global_var_2 = 0x22222222;
    global_var_3 = 0x33333333;
    global_var_4 = 0x44444444;
    global_var_5 = 0x55555555;

    // Mix access to initialized variable
    init_var_1 = 0xDEADBEEF;

    return 0;
}

static void __exit reloc_test_exit(void) {
    // Access again mixed
    global_var_1 = 0xAAAAAAAA;
}

module_init(reloc_test_init);
module_exit(reloc_test_exit);
MODULE_LICENSE("GPL");
