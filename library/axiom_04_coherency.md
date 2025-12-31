# Axiom 04: The Exclusion Principle

**Subject**: Concurrency, Caches, and Locks
**Derived From**: `ptlock_split`

---

## 1. The Hardware Constraint
*   **Axiom**: A Cache Line (64 bytes) can be Modified (State 'M' in MESI) by only **one** CPU at a time.
*   **Implication**: If CPU A writes to Address X, and CPU B writes to Address X, they must serialize.

## 2. Derivation: The Cost of Sharing

### The Bouncing Line
1.  CPU A claims ownership of Line L. Writes.
2.  CPU B requests ownership of Line L.
3.  Hardware forces CPU A to flush L to L3 Cache.
4.  CPU B reads L from L3. Writes.
5.  **Result**: 100x latency penalty.

### The Software Lock
*   **Problem**: Read-Modify-Write is not atomic.
*   **Solution**: Spinlocks. A variable that leverages the hardware exclusion to create a "Waiting Room" for software.

## 3. The Consequence
*   **Axiom**: Global counters are slower than local counters.
*   **Optimization**: Split the data. Instead of one Global Lock, have 1000 Local Locks (Split Page Table Locks). Minimize the collision surface.
