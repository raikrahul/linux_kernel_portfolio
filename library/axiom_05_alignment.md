# Axiom 05: The Power of Two

**Subject**: Binary Math, Alignment, and The Buddy System
**Derived From**: `buddy_fragment`

---

## 1. The Binary Axiom
*   **Statement**: Computer memory is addressed in base-2.
*   **Implication**: Boundaries and sizes that are powers of 2 ($2^n$) allow arithmetic optimization.

## 2. Derivation: The Bitwise Shortcuts

### Division and Modulo
*   **Decimal**: $X / 4096$ is slow.
*   **Binary**: $X >> 12$ is instant (1 cycle).
*   **Decimal**: $X \% 4096$ is slow.
*   **Binary**: $X \& (4096 - 1)$ is instant (1 cycle).

### The XOR Buddy (The Ultimate Derivation)
*   **Problem**: Find the neighbor of a memory block of size $2^n$ at address $P$.
*   **Math**: The neighbor's address differs by exactly one bit (the $n$-th bit).
*   **Formula**: `Buddy = P ^ (1 << n)`.

## 3. The Consequence
*   **Axiom**: Structure sizes should be aligned to cache lines (64 bytes) or pages (4096 bytes).
*   **Proof**: The `page->flags` packing relies on the address space being aligned so that the bottom bits are always zero (used for tags).
