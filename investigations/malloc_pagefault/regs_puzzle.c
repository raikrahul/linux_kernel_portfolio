#include <stdio.h>

struct registers {
  unsigned long r0;
  unsigned long r1;
  unsigned long r2;
  unsigned long r3;
  unsigned long r4;
  unsigned long r5;
  unsigned long r6;
  unsigned long r7;
  unsigned long r8;
  unsigned long r9;
};

/* TODO: Implement the numerical access logic */
/* Goal: Return the value at 'base + offset' without using member names */
unsigned long get_reg(struct registers *regs, unsigned int offset) {
  // 1. Convert regs pointer to an integer number (long)
  // 2. Add the byte-offset to that number
  // 3. Convert the resulting number back into a (long *) pointer
  // 4. Dereference it and return
  return *(unsigned long *)((unsigned long)(regs) + offset);
}

int main() {
  struct registers context = {.r0 = 0x00,
                              .r1 = 0x11,
                              .r2 = 0x22,
                              .r3 = 0x33,
                              .r4 = 0x44,
                              .r5 = 0x55,
                              .r6 = 0x66,
                              .r7 = 0x77,
                              .r8 = 0x88,
                              .r9 = 0x99};

  printf("Base: %p\n", (void *)&context);

  // Test 1: Offset 24 should be r3 (0x33)
  unsigned long v3 = get_reg(&context, 24);
  printf("Offset 24: 0x%lx (Expected 0x33)\n", v3);

  // Test 2: Offset 56 should be r7 (0x77)
  unsigned long v7 = get_reg(&context, 56);
  printf("Offset 56: 0x%lx (Expected 0x77)\n", v7);

  return 0;
}
