# TASK ANALYSIS & PLANNING REPORT

## 1. PRIMARY OBJECTIVE
**Goal**: Master Linux Kernel Bitmasking & Page Table Walking via Axiomatic Derivation.
**Constraint**: 0 English words in derivations. 100% Numerical. No "Thinking".
**Current State**: User blocked on bitmasking inputs/outputs.
**Root Cause**: Lack of mental model for `(Value >> Shift) & Mask` visualization.

## 2. TASK GRILLING (INTERROGATING THE REQUEST)

01. **Constraint Conflict**: "No abstractions" vs "Distinct Puzzles".
    - *Resolution*: Puzzles must use real data (e.g., Filesystem Blocks, IP Subnets) not abstract "Apples".

02. **Verification Gap**: How to verify manual bitmasking?
    - *Plan*: User must verify `((A >> B) & C)` against Python/Calculator at every step.
    - *Risk*: User skips verification. *Mitigation*: Explicit "Check: X = Y ✓" lines.

03. **Scope Creep**: "Memory Management" is too broad.
    - *Focus*: Specific focus on *Address Translation* (PML4 -> PDPT -> PD -> PT -> Frame).
    - *Exclusion*: No `malloc`, no `mmap`, no allocators yet.

04. **Format Rigidity**: "Dense paragraphs" vs "Wide screen".
    - *Constraint*: Lines must be 100-140 chars long.
    - *Format*: `Input: 0x1234 → Bin: 0001... → Shift: 0x12 → Mask: 0x2 → Idx: 2` (Horizontal flow).

## 3. CONCEPTUAL ANALYSIS (NUMERICAL METAPHORS)

### SUBJECT: HIERARCHICAL BITMASKING (The "Problem")

#### CONCEPT A: LEFT/RIGHT SHIFTING (Information Extraction)
**Metaphor**: Decimal System Digit Extraction.
**Distinct Domain**: Time (Seconds/Minutes/Hours) from Total Seconds.

- **What**:
    - Input: 3,723 seconds.
    - Op: `3723 // 3600 = 1`. `(3723 % 3600) // 60 = 2`. `3723 % 60 = 3`.
    - Output: {Index₃:1, Index₂:2, Index₁:3}.
    - Kernel Equiv: `(Addr >> 30) & 0x1FF`, `(Addr >> 21) & 0x1FF`.

- **Why**:
    - 3,723 is 1 number. 1:02:03 is 3 coordinates.
    - 48-bit Addr is 1 number. 4 Indices are 4 coordinates.
    - Ratio: 1 huge number → 4 small indices (9-bit each).

- **Where**:
    - Digital Clocks (Time → Display).
    - Cartesian Coordinates (Linear Index → X,Y,Z).
    - CPU: Virtual Address 0x7FFFFF... → Physical 0x1000.

- **Who**:
    - 1 Divider (Shift Unit).
    - 1 Modulo (Mask Unit).
    - 1 Accumulator (Result Register).

- **When**:
    - Every 10µs (Clock tick).
    - Every Memory Access (Page Walk).

- **Without**:
    - 1 Linear second counter (Unix Epoch) requires 5GB lookup table.
    - Hierarchical: 3 small tables (Hour/Min/Sec).
    - Space saving: 99.999% reduction (Sparse population).

- **Which**:
    - Decimal: Base 10 (Div 10, 60).
    - Binary: Base 2 (Shift 9, Mask 0x1FF).
    - Binary is 100× faster in HW (Wires vs Logic Gates).

#### CONCEPT B: BITWISE AND (Masking/Filtering)
**Metaphor**: IP Subnetting (CIDR).
**Distinct Domain**: Networking.

- **What**:
    - Input: IP 192.168.1.55 (11000000.10101000.00000001.00110111).
    - Mask: /24 (255.255.255.0) (11111111.11111111.11111111.00000000).
    - Op: `IP & Mask`.
    - Output: Network 192.168.1.0. Host 0.0.0.55.

- **Why**:
    - Separation of concerns: Network (Routing) vs Host (Local Delivery).
    - Kernel Equiv: VPN (Page Table Index) vs Offset (Byte in Page).

- **Where**:
    - Routers (Packet switching).
    - ACLs (Firewalls).
    - Memory Controller (Row/Column selection).

- **When**:
    - Packet arrival (Input).
    - Address decode (Input).

- **Without**:
    - Global routing table size = $2^{32}$ entries (4 billion).
    - With Masking: Routing table size = 800,000 entries (Aggregation).

- **Which**:
    - High Bits = Path.
    - Low Bits = Destination.
    - PAGE_MASK (High) vs ~PAGE_MASK (Low).

## 4. CONCRETE NUMERICAL PUZZLE (DISTINCT DOMAIN)

### PUZZLE: THE DECIMAL LIBRARY INDEXER
**Goal**: Locate Book #584,291 in a 3-Level Library.
**Constraints**:
- **Level 1 (Floor)**: Holds 100 Rooms (00-99).
- **Level 2 (Room)**: Holds 100 Shelves (00-99).
- **Level 3 (Shelf)**: Holds 100 Books (00-99).

**System State**:
- **Root Pointer**: Floor Map is at Physical Address 1000.
- **Table 1 (Floors)**: `[Index] → Room_Table_Addr`.
    - Entry 58: Points to 2000.
- **Table 2 (Rooms)**: `[Index] → Shelf_Table_Addr`.
    - Entry 42: Points to 3000.
- **Table 3 (Shelves)**: `[Index] → Book_Phys_Addr`.
    - Entry 91: Points to 5000.

**Trace**:
1. **Input ID**: 584,291.
2. **Decomposition**:
   - **Level 1 (Floor) Index**: `ID // 100 // 100` = `584291 // 10000` = **58**.
   - **Remainder 1**: `584291 % 10000` = **4291**.
   - **Level 2 (Room) Index**: `4291 // 100` = **42**.
   - **Remainder 2**: `4291 % 100` = **91**.
   - **Level 3 (Shelf) Index**: **91**.

3. **Walk**:
   - **Step 1**: Start Root(1000). Read Entry[58]. Value = 2000.
   - **Step 2**: Go to 2000. Read Entry[42]. Value = 3000.
   - **Step 3**: Go to 3000. Read Entry[91]. Value = 5000.
   - **Result**: Physical Address 5000.

**Kernel Equivalence**:
- `// 10000` $\approx$ `>> 30`.
- `// 100` $\approx$ `>> 21`.
- `// 1` $\approx$ `>> 12`.
- Base 10 logic is identical to Base 2 logic (Bitmasking).

## 5. SUB-TASKS REPORT (EXPECTED WORK)

To solve "Cannot do bitmaskings", the following sub-tasks are mandatory:

1. **Binary Fluency Drill**:
   - Task: Convert Hex 0x0...0xFF to Binary by hand 50 times.
   - Why: Pattern recognition (0xC = 1100).

2. **Manual Shifting Drill**:
   - Task: Calculate `0x123456789 >> 12` by hand.
   - Why: Understand "Nibble Dropping". 12 bits = 3 Hex digits.
   - `0x123456789 >> 12` → `0x123456` (Drop 789).

3. **Manual Masking Drill**:
   - Task: `0x123456789 & 0x1FF`.
   - Why: Understand "Modulus". 0x1FF = 511 (9 bits).
   - Result: Extract last 9 bits.

4. **Page Walk Simulation (Paper)**:
   - Task: Given CR3 and VAddr, write the 4-step lookups.
   - Requirement: No variables. Real addresses only.

## 6. FAILURE PREDICTIONS (PRE-MORTEM)

:01. Failure: User confuses `>> 12` with `/ 12`.
    - Impact: Wrong Index.
    - Fix: `>> N` is `/ 2^N`. `>> 12` is `/ 4096`.

:02. Failure: User forgets to handle Indices as *Offsets* × 8.
    - Input: Index 5. Address = Base + 5? NO.
    - Actual: Address = Base + (5 × 8 bytes).
    - Fix: Always write `Base + (Index * 8)`.

:03. Failure: User misinterprets "Canonical Form" (Sign extension).
    - Issue: Bits 48-63 must copy Bit 47.
    - Fix: Explicit check `if (bit 47) top = 1s else top = 0s`.

:04. Failure: Hex Math errors (C + 8 = ?).
    - Issue: C(12) + 8 = 20 = 0x14. User writes 0x20.
    - Fix: Draw Hex Clock (0-F) for addition.

:05. Failure: Little Endian confusion.
    - Issue: Memory traces show `EF CD AB 89`. User reads `0xEFCD...`.
    - Actual: `0x89ABCDEF`.
    - Fix: Explicit "Byte Swap" step in every memory read.
