# Investigation: Tracing Early Boot Memory

**Date**: 2025-12-28
**Subject**: Bootmem & E820 Map
**Machine**: x86_64, 12 CPUs

---

## 1. Axiomatic Foundation

### The BIOS/Kernel Handoff
*   **AXIOM**: When power is applied, the kernel is not yet running. The Firmware (BIOS/UEFI) controls the hardware.
*   **AXIOM**: The Kernel needs to know *what* memory exists before it can manage it.
*   **MECHANISM**: The E820 Map. A primitive list of memory ranges provided by firmware.

## 2. Derivation: The Hardware Map

We trace the physical memory layout as reported by the kernel during early boot.

### Observation (Live Data)
*   **SOURCE**: `dmesg | grep BIOS-e820`
*   **DATA**:
    ```text
    [    0.000000] BIOS-e820: [mem 0x0000000000000000-0x000000000009fbff] usable
    [    0.000000] BIOS-e820: [mem 0x000000000009fc00-0x000000000009ffff] reserved
    ...
    [    0.000000] BIOS-e820: [mem 0x0000000100000000-0x000000043fffffff] usable
    ```

### Interpretation
*   **0x0000 - 0x9FBFF**: The "Low Memory" (Legacy Real Mode area). ~640KB.
*   **0x100000000+**: Memory above 4GB (High Memory).

## 3. The Problem: Allocating Before the Allocator
*   **PARADOX**: To initialize the sophisticated memory allocator (Buddy System), the kernel needs to allocate metadata structures (`struct page`).
*   **QUESTION**: Where does the memory for the metadata come from, if the allocator isn't running yet?
*   **SOLUTION**: `memblock` (formerly `bootmem`). A primitive, first-fit allocator that operates directly on the E820 ranges.

## 4. Experiment: Kernel Module Trace
We wrote a module to inspect the state of memory *after* boot.

*   [bootmem_trace.c](https://github.com/raikrahul/linux_kernel_portfolio/blob/main/investigations/bootmem_trace/bootmem_trace.c): A kernel module to print current memory zones.

### derived_trace.log
```text
Zone DMA      : 0x00001000 -> 0x0000xxxx
Zone DMA32    : 0x00100000 -> 0xC0000000
Zone Normal   : 0xC0000000 -> ...
```

## 5. Conclusion
The machine's memory is not a clean slate. It is a Swiss-Cheese of reserved ranges (ACPI, PCI holes, BIOS data). The kernel stitches these playable fragments into the "Zones" we see in `dmesg`.
