/*
 * asm_demo.c - Demonstrates inline asm constraint behavior
 * Compile: gcc -O0 -o asm_demo asm_demo.c
 * Run: ./asm_demo
 */
#include <stdio.h>

int main() {
    unsigned long good_result = 0;
    unsigned long garbage_result = 0;
    unsigned long input_value = 0xDEADBEEFCAFEBABE;

    printf("DEMO: Inline Assembly Constraints\n");
    printf("══════════════════════════════════════════════════════════\n\n");

    /* CORRECT: Use %0 to write to output operand */
    printf("1. CORRECT USAGE: mov input to output via %%0\n");
    asm("mov %1, %0" : "=r"(good_result) : "r"(input_value));
    printf("   Input:  0x%016lx\n", input_value);
    printf("   Output: 0x%016lx\n", good_result);
    printf("   Match?  %s\n\n", (good_result == input_value) ? "YES ✓" : "NO ✗");

    /* WRONG: Declare =r but never use %0 in asm */
    printf("2. WRONG USAGE: nop but claim output with =r\n");
    garbage_result = 0x1111111111111111; /* Set known value first */
    printf("   Before asm: 0x%016lx\n", garbage_result);
    asm("nop" : "=r"(garbage_result)); /* nop doesn't write to %0! */
    printf("   After asm:  0x%016lx\n", garbage_result);
    printf("   Expected:   GARBAGE (random register value)\n\n");

    /* ANOTHER CORRECT EXAMPLE: actually compute something */
    printf("3. CORRECT USAGE: add two values\n");
    unsigned long a = 100, b = 200, sum = 0;
    asm("add %1, %0" : "=r"(sum) : "r"(a), "0"(b)); /* sum = b + a */
    printf("   %lu + %lu = %lu\n\n", a, b, sum);

    printf("══════════════════════════════════════════════════════════\n");
    printf("LESSON: '=r' is a PROMISE that you write to %%0.\n");
    printf("        If you don't use %%0, compiler still stores garbage.\n");

    return 0;
}
