# Investigation: NUMA Zones and Fallbacks

**Date**: 2025-12-28
**Subject**: NUMA (Non-Uniform Memory Access) Topology
**Machine**: x86_64, 12 CPUs (1 Socket, 1 NUMA Node)

---

## 1. Axiomatic Foundation

### The Speed of Light
*   **AXIOM**: Electricity travels at finite speed.
*   **IMPLICATION**: Distance matters. RAM closer to the CPU is faster than RAM further away.

### NUMA Architecture
*   **DEFINITION**: A system where memory is partitioned into "Nodes".
*   **LOCAL MEMORY**: Attached directly to the CPU's memory controller (Fast).
*   **REMOTE MEMORY**: Accessed via interconnect (Slow).

## 2. Derivation: The Zoning Hierarchy

Linux divides physical RAM into Zones based on hardware limitations.

### The Zones
1.  **ZONE_DMA** (< 16MB): For ancient 24-bit DMA devices.
2.  **ZONE_DMA32** (< 4GB): For 32-bit devices.
3.  **ZONE_NORMAL** (> 4GB): All other RAM (12GB on this machine).

### The Allocation Fallback
When a request comes for `ZONE_NORMAL`, what if it's full?
*   **AXIOM**: A 32-bit address is a subset of a 64-bit address.
*   **DERIVATION**: Allocation can "fall back" to lower zones.
*   **ORDER**: `Normal` → `DMA32` → `DMA`.
*   **INVERSE**: A `DMA` request CANNOT be satisfied by `Normal` memory (hardware cannot address it).

## 3. Experiment: Tracing the Layout

*   [numa_zone_trace.c](./numa_zone_trace.c): Kernel module to walk the `pg_data_t` (Node) and `zone_struct` hierarchy.
*   **OBSERVATION** (Live Data):
    *   Node 0 present.
    *   Node 0 has Zones: DMA, DMA32, Normal.
    *   `nr_spanned_pages` matches physical RAM layout.

## 4. Conclusion
On this single-socket workstation, NUMA is degenerate (1 Node). However, the kernel still treats it as a "NUMA system of one", using the same `pg_data_t` abstractions.
