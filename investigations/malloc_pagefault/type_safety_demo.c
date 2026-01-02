// Without Structs (DANGEROUS)
typedef unsigned long pte_t;
typedef unsigned long pgd_t;

void set_pte(pte_t *entry, unsigned long val);

unsigned long my_pgd = 0x1234;
set_pte(&my_pgd,
        0x5678); // COMPILES! BUT WRONG! (Corrupting PGD with PTE value)

// With Structs (SAFE)
typedef struct {
  unsigned long val;
} pte_t;
typedef struct {
  unsigned long val;
} pgd_t;

void set_pte(pte_t *entry, unsigned long val);

pgd_t my_pgd = {0x1234};
set_pte(&my_pgd, 0x5678); // COMPILER ERROR: "incompatible type"
