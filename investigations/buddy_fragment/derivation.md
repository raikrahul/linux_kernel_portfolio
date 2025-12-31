# Investigation: The Buddy Allocator

**Date**: 2025-12-28
**Subject**: Physical Memory Fragmentation & The Buddy System
**Machine**: x86_64, 12 CPUs

---

## 1. Axiomatic Foundation

### The Problem of Contiguity
*   **AXIOM**: Software often requires contiguous physical memory (e.g., for DMA buffers).
*   **AXIOM**: RAM is fragmented over time as pages are allocated and freed in random order.
*   **PROBLEM**: How to satisfy a request for 8 contiguous pages (32KB) if only scattered single pages are free?

### The Power of Two
*   **AXIOM**: Binary systems work best with powers of 2.
*   **MECHANISM**: The Buddy System. Memory is managed in blocks of size $2^n$ pages (Order $n$).
*   **RANGE**: Order 0 (4KB) to Order 10 (4MB).

## 2. Derivation: The Buddy Formula

How does the kernel instantly find the "buddy" of a block to merge with? It derives it mathematically, without searching lists.

### The XOR Insight
*   **GIVEN**: A block at PFN $P$ of Order $n$.
*   **DEFINITION**: Its unique buddy is the block of same size that makes up the aligned parent ($n+1$).
*   **OBSERVATION**:
    *   If $P$ is the "left" half, Buddy is $P + 2^n$.
    *   If $P$ is the "right" half, Buddy is $P - 2^n$.
*   **DERIVATION**:
    *   In binary, $2^n$ has a single bit set at position $n$.
    *   Toggling this bit switches between "left" and "right" halves.
*   **FORMULA**:
    ```c
    BuddyPFN = PFN ^ (1 << Order)
    ```

### Numerical Proof
*   **GIVEN**: PFN = 1000, Order = 3 (8 pages).
*   **CALCULATION**:
    1.  `1 << 3` = 8 (Binary `1000`)
    2.  PFN 1000 (Binary `1111101000`)
    3.  `1111101000` XOR `0000001000` = `1111100000`
    4.  Result: 992.
*   **CONCLUSION**: The buddy of PFN 1000 is PFN 992. They merge to form a 16-page block at PFN 992.

## 3. Experiment: Visualizing Fragmentation

We verify the state of the buddy lists on the live machine.

*   **SOURCE**: `/proc/buddyinfo`
*   **OBSERVATION**:
    ```text
    Node 0, Zone Normal: 25281 22621 4377  854  383  177   78   40    9    6    3
    ```
*   **INTERPRETATION**:
    *   Order 0 (4KB): 25,281 blocks free.
    *   Order 10 (4MB): Only 3 blocks free.
    *   **CONCLUSION**: Memory is highly fragmented. A request for 8MB (Order 11) would fail immediately (MAX_ORDER=11, max allocation is Order 10).

## 4. The Code
*   [buddy_fragment.c](https://github.com/raikrahul/linux_kernel_portfolio/blob/main/investigations/buddy_fragment/buddy_fragment.c): Kernel module that attempts to allocate high-order pages and measures failure rates.

## 5. Conclusion
The Buddy System trades internal fragmentation (wasted space inside 4MB blocks) for external fragmentation resistance and O(1) merge speed using the XOR axiom.
