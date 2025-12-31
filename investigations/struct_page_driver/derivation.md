# Investigation: The Struct Page Metadata

**Date**: 2025-12-28
**Subject**: `struct page` & `vmemmap`
**Machine**: x86_64, 16GB RAM

---

## 1. Axiomatic Foundation

### The Shadow Array
*   **AXIOM**: We have 3,944,069 physical pages of RAM.
*   **PROBLEM**: The kernel needs to track the status of *every single one*.
*   **SOLUTION**: An array of metadata structures, parallel to physical memory.
*   **NAME**: `vmemmap`.

### The cost of Tracking
*   **OBSERVATION**: `sizeof(struct page)` = 64 bytes.
*   **CALCULATION**:
    ```text
    3,944,069 pages × 64 bytes = 252,420,416 bytes
    ≈ 240 MB
    ```
*   **CONCLUSION**: 1.5% of total RAM is consumed just to track the rest of RAM.

## 2. Derivation: vmemmap Arithmetic

We can derive the location of any page's metadata using simple pointer arithmetic.

### The Formula
*   **GIVEN**: `VMEMMAP_START` (Architecture constant, e.g., `0xffffea0000000000`).
*   **GIVEN**: PFN (Page Frame Number).
*   **DERIVATION**:
    ```c
    struct page *page = (struct page *)VMEMMAP_START + pfn;
    ```
    (Note: C pointer arithmetic automatically scales by `sizeof(struct page)`).
*   **RAW MATH**:
    ```
    Address = Start + (PFN * 64)
    ```

## 3. Experiment: Finding Myself

We wrote a module that takes the kernel address of its own code and finds the corresponding metadata.

*   [page_labs.c](https://github.com/raikrahul/linux_kernel_portfolio/blob/main/investigations/struct_page_driver/page_labs.c): Module using `virt_to_page()`.
*   **TRACE**:
    1.  Variable `x` at `0xffff...`.
    2.  Subtract `PAGE_OFFSET` → Physical Address.
    3.  Shift Right 12 → PFN.
    4.  Add to `vmemmap` → `struct page` pointer.
    5.  Read `page->_refcount` → 1 (The module itself holding it).

## 4. Conclusion
The `struct page` is the atom of Linux memory management. It is the "Jail" that holds the PFN inmate. All higher-level allocators (Buddy, SLAB, vmalloc) ultimately deal in currency of `struct page`.
