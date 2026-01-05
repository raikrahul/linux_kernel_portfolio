/*
 * asm_operands.c - Multiple operand inline assembly demo
 * Compile: gcc -O0 -S -o asm_operands.s asm_operands.c
 * Compile: gcc -O0 -o asm_operands asm_operands.c
 */
#include <stdio.h>

int main() {
    unsigned long a = 100;
    unsigned long b = 200;
    unsigned long sum = 0;

    /*
     * asm("add %1, %0" : "=r"(sum) : "r"(a), "0"(b));
     *          ↑   ↑     ↑          ↑       ↑
     *          │   │     │          │       └── "0" = same register as operand 0
     *          │   │     │          └── operand 1 = input, a
     *          │   │     └── operand 0 = output, sum
     *          │   └── %0 = register for operand 0
     *          └── %1 = register for operand 1
     *
     * WHAT HAPPENS:
     *   1. Compiler picks rax for operand 0 (sum)
     *   2. Compiler picks rcx for operand 1 (a)
     *   3. "0"(b) means: load b into SAME register as operand 0 → rax
     *   4. Before asm: rax = 200 (b), rcx = 100 (a)
     *   5. Execute: add %rcx, %rax → rax = rax + rcx = 200 + 100 = 300
     *   6. Store rax to sum → sum = 300
     */
    asm("add %1, %0" : "=r"(sum) : "r"(a), "0"(b));

    printf("a = %lu, b = %lu\n", a, b);
    printf("sum = %lu (expected: 300)\n", sum);
    printf("Match? %s\n", (sum == 300) ? "YES" : "NO");

    return 0;
}
