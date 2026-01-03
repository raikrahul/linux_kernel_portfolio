#include <stddef.h>
#include <stdio.h>

/* Mock pt_regs for practice */
struct mock_regs {
  unsigned long rA;
  unsigned long rB;
  unsigned long rC;
  unsigned long rD;
  unsigned long rE;
};

/* The logic being practiced */
unsigned long get_reg_at_offset(struct mock_regs *regs, unsigned int offset) {
  /* Step 1: Cast pointer to number for byte-step addition */
  unsigned long base_addr = (unsigned long)regs;

  /* Step 2: Add byte-offset to base */
  unsigned long target_addr = base_addr + offset;

  /* Step 3: Cast back to pointer and dereference */
  return *(unsigned long *)target_addr;
}

int main() {
  struct mock_regs data = {
      .rA = 0xAA, .rB = 0xBB, .rC = 0xCC, .rD = 0xDD, .rE = 0xEE};

  printf("Base Address: %p\n", (void *)&data);
  printf("Offset of rC: %zu (expected 16)\n", offsetof(struct mock_regs, rC));

  unsigned long val = get_reg_at_offset(&data, 16);
  printf("Value at offset 16: 0x%lx (expected 0xCC)\n", val);

  return 0;
}
