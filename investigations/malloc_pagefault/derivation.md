# Investigation: Malloc is a Lie

**Date**: 2025-12-29
**Subject**: Virtual Memory, `malloc`, and Page Faults
**Machine**: x86_64

---

## 1. Axiomatic Foundation

### The Promise
*   **AXIOM**: `malloc(N)` promises N bytes of memory.
*   **REALITY**: The kernel promises nothing but a *range of addresses*. Physical RAM is NOT allocated until you touch it.

## 2. Derivation: The Page Fault Mechanism

1.  **User**: `ptr = malloc(4096)`.
2.  **Kernel**: Updates VMA (Virtual Memory Area) list. "User owns 0x7f... to 0x7f...+4096". Returns address. **RAM Used: 0**.
3.  **User**: `*ptr = 'A'`.
4.  **Hardware**: CPU sees address. TLB Miss. Page Table Walk.
5.  **MMU**: Entry is Empty (Not Present).
6.  **Exception**: Page Fault (#PF) triggers kernel handler.
7.  **Kernel Handler**:
    *   Check VMA: Is this valid? Yes.
    *   Alloc Page: `alloc_page(GFP_HIGHUSER)`.
    *   Update Page Table: Point PTE to new physical page.
    *   Return from Exception.
8.  **User**: Instruction re-executes. Write succeeds.

## 3. Experiment: Proof of Laziness

*   [malloc_fault.c](./malloc_fault.c): Program to allocate 1GB and measure RAM usage before/after writing.
*   **OBSERVATION**:
    *   `malloc(1GB)` -> RES (Resident Memory) = 0 KB.
    *   `memset(ptr, 0, 1GB)` -> RES rises to 1GB.

## 4. Conclusion
Memory allocation is a negotiation between the Process (Virtual) and the Kernel (Physical), mediated by the Page Fault. `malloc` reserves the *right* to memory; `access` claims the *reality* of it.
