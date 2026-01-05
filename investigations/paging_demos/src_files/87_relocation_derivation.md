
# Axiomatic Derivation: Relocation & Static Variables

## Axiom 1: Instructions Must Exist in Memory
1. CPU executes instructions one by one.
2. Instructions are stored in RAM.
3. RAM is a linear array of bytes: `0x0`, `0x1` ... `0xFFFFFF...`.
4. To execute, the Code bytes must hold a specific address `A_code` (e.g., `0x1000`).

## Axiom 2: Data Must Exist in Memory
1. `unsigned long target_vaddr` is data (8 bytes).
2. It must live in RAM to be read/written.
3. It holds a specific address `A_data` (e.g., `0x2000`).

## Axiom 3: Instructions Access Data
1. An instruction `MOV` copies value from `A_data` to a register.
2. The instruction itself is just bytes: `[OPCODE][ADDRESS]`.
3. Example: `48 8B 05 ...` (Move 8 bytes from relative address ...).

## Derivation 4: The Separation of Creation and Use (Time)
1. **Creation Time (Compilation)**:
   - Tool: `gcc` (Compiler).
   - Input: Source Text (`pagewalk_driver.c`).
   - Output: File on Disk (`pagewalk_driver.ko`).
   - Limit: The computer is OFF or running other things. RAM for the module is NOT allocated yet.
   - Fact: The file on disk does not have a RAM address. It is at `Sector 123` on the hard drive. ✗

2. **Use Time (Loading)**:
   - Tool: `insmod` (Loader).
   - Input: File on Disk.
   - Output: Live Code in RAM.
   - Action: OS finds free space in RAM.
   - Fact: Free space location is RANDOM/DYNAMIC depending on what else is running.
     - Run 1: Free space at `0x1000`.
     - Run 2: Free space at `0x5000`.

## Derivation 5: The Address Uncertainty Principle
1. Axiom: The Compiler runs BEFORE the Loader.
2. Axiom: The Compiler must write the machine code *now*.
3. Axiom: Machine code instructions like `MOV` require a specific numeric address to access data.
4. Problem:
   - Compiler needs to write `MOV [ADDRESS], %rax`.
   - Compiler asks: "What is [ADDRESS]?"
   - Truth: [ADDRESS] does not exist yet. It will only exist later, at Load Time.
5. Inference: The Compiler CANNOT write the correct address.
   - It is logically impossible.
   - It faces a "Future Value" problem.
   - Therefore, it MUST write a placeholder.

## Derivation 6: The CPU's Cursor (RIP/IP)
1. **Axiom**: To execute a book of instructions, you need a bookmark.
2. **Definition**: The CPU has a register called **RIP** (Instruction Pointer).
   - Value = Memory Address of the *Instruction currently being executed*.
   - Example: If executing the instruction at `0x1000`, RIP = `0x1000`.
3. **Mechanic**: After running the instruction, CPU adds instruction length to RIP.
   - `MOV` instruction length = 7 bytes.
   - Next RIP = `0x1000 + 7` = `0x1007`.

## Derivation 7: Relative Addressing (The "Neighbors" Concept)
1. **Scenario**: You are standing at `0x1000` (Code). You want to reach `0x5000` (Data).
2. **Absolute Way**: "Go to `0x5000`." (Requires knowing `0x5000`).
3. **Relative Way**: "Go forward `0x4000` bytes." (Requires knowing the *Distance*).
   - Distance = Target(`0x5000`) - Current(`0x1000`) = `0x4000`.
4. **Assembly Syntax**: `[RIP + 0x4000]`.
   - Meaning: "Take my current location, add 0x4000, and look there."
   - Why use this? It allows the whole Code+Data block to shift (e.g., Load at `0x2000`) without changing the *internal distance* (`0x4000`).

## Derivation 8: The "Distance Unknown" Problem (Cross-Section)
1. **Fact**: Code lives in the `.text` section.
2. **Fact**: `target_vaddr` lives in the `.bss` section (because it is uninitialized/zero).
3. **Refinement**:
   - Compiler builds `.text` block (e.g., 100 bytes).
   - Compiler builds `.bss` block request (e.g., 8 bytes).
   - Compiler does **NOT** decide how these two blocks are placed in RAM.
   - The Loader decides placement.
     - Case A: Loader puts `.bss` right after `.text`. (Distance = 100).
     - Case B: Loader puts `.bss` far away at `0x100000`. (Distance = Huge).
4. **Conflict**:
   - Compiler must write the `MOV [RIP + DISTANCE]` instruction NOW.
   - But `DISTANCE` depends on Case A vs Case B (Loader decision).
   - Therefore, `DISTANCE` is **Unknown** at compile time.

## Derivation 9: The Placeholder (Why Zero?)
1. **Logic**: The instruction needs a 4-byte number for the Distance.
2. **Constraint**: We don't know the number yet.
3. **Action**: We must reserve space for it in the instruction bytes.
4. **Choice**: Write `00 00 00 00` (Zero).
   - "Placeholder Offset".
   - Current Instruction becomes: `MOV [RIP + 0]`.
   - Does this mean "Access memory at current IP"? **Yes.**
   - Is this wrong? **Yes.** It is temporary garbage. It MUST be fixed before running.
5. **Quantity**: If you have 1000 global variables, you have 1000 `MOV` instructions.
   - Each one has an unknown distance to its specific variable.
   - Each one gets its own `00 00 00 00` placeholder.
   - Each one gets a **Relocation Note** saying "Please fix this specific zero later."

## The Solution: Relocation
1. The compiler generates the code with `0x0`.
2. The compiler creates a **Relocation Entry** (a note) in the file.
3. **The Note Contents**:
   - "At Offset `0xF7` in the code section..."
   - "There is a 4-byte value..."
   - "It needs to point to symbol `target_vaddr`."

## The Loader's Job (Runtime)
1. **Allocation**:
   - Loader asks kernel: "Give me memory for Code." -> Gets `A_code = 0xFFFF...1000`.
   - Loader asks kernel: "Give me memory for Data (.bss)." -> Gets `A_data = 0xFFFF...5000`.

2. **Patching (Relocation Application)**:
   - Loader reads the Note.
   - Loader calculates **Current Position**: `P = A_code + 0xF7` (Where the instruction is).
   - Loader calculates **Target**: `A_data`.
   - Loader calculates **Relative Offset**: `Offset = A_data - (P + 4)` (RIP-relative math: Address of Next Instruction).
   - Loader **Overwrites** the `0x0` in memory with `Offset`.

## Result
1. The memory at `0xFFFF...10F7` now contains the correct offset.
2. The CPU executes `mov %rsi, [RIP + Offset]`.
3. [RIP + Offset] correctly resolves to `A_data`.
4. The value is stored in `target_vaddr`.

## Summary Chain
1. Unknown Load Address -> Requires Placeholder in Code (0x0).
2. Placeholder -> Requires Metadata (Relocation Entry).
3. Metadata + Runtime Addresses -> Enables Loader to Calculate Offset.
4. Calculated Offset -> Overwrites Placeholder (Patching).
5. Patched Code -> Correct Execution.

## Derivation 10: The Evidence Trace (Concrete Proof)
1. **Fact (Source)**: `global_var_1 = 0x11111111;`
2. **Fact (Assembly Line 1b)**: `48 c7 05 00 00 00 00`
   - `48 c7 05` = Instruction "MOV".
   - `00 00 00 00` = The **Placeholder** 0x0. ✓
3. **Fact (Relocation Line 1e)**: `R_X86_64_PC32 .bss+0x18`
   - `1e` = The address OF the zero bytes (1b + 3 bytes).
   - `.bss+0x18` = The **target** definition.
4. **Calculated Patching (Runtime Simulation)**:
   - Assume `.bss` starts at `0xF000` (Loader decision).
   - `global_var_1` is at `.bss + 0x18` = `0xF018`.
   - Code is at `0x1000`.
   - Instruction Pointer (RIP) at next line = `0x1000 + 1b + 7` = `0x1022` (approx).
   - Distance = Target - RIP = `0xF018 - 0x1022` = `0xDFF6`.
   - **Action**: Loader overwrites `00 00 00 00` with `F6 DF 00 00` (Little Endian).
   - **Result**: Code becomes valid instructions pointing to the variable.

## Derivation 11: The "P + 4" Axiom (RIP Mechanics)
1. **Axiom (Fetch-Decode-Execute)**:
   - CPU fetches Instruction bytes.
   - CPU increments RIP **during** fetch/decode of current instruction length.
   - RIP points to **Next Instruction** when execution begins.

2. **Instruction Anatomy (MOV relative)**:
   - Byte 0-2: Opcode () [3 bytes]
   - Byte 3-6: Offset () [4 bytes]
   - Total Length = 7 bytes.
   -  (Position of Patch) = Start + 3.

3. **Why (P + 4)?**:
   - We need distance from **End of Instruction** (RIP).
   - Patch is at .
   - Patch size is  bytes.
   - End of Instruction = .
   - End of Instruction =  = .
   - $\therefore$ RIP = P + 4.

4. **Calculation**:
   - Target = RIP + Offset.
   - Offset = Target - RIP.
   - Offset = Target - (P + 4).

## Derivation 12: The Definition of a Constraint (Relocation Record)
1. **Axiom (Freedom vs Rules)**:
   - "Freedom": The Loader can place the `.bss` section ANYWHERE in RAM (e.g., `0x1000`, `0x9000`).
   - "Rule": The Code `MOV [RIP+X]` *must* point to that exact location.

2. **The Connection**:
   - The value `X` (the offset in the instruction) is **dependent** on the Loader's choice.
   - Equation: $X_{code} = Address_{bss} - Address_{code}$.

3. **The Constraint Object**:
   - The Compiler leaves a record in the file: "I have left a hole at file offset `1e`."
   - "This hole MUST satisfy the equation: $HoleValue = TargetAddress - CurrentAddress$."
   - This record is the **Constraint**.
   - Input: The loader's decision ($Address_{bss}$).
   - Requirement: The code instruction must match the decision.

4. **Visualizing the Constraint entry**:
   - `Location`: "Byte 30 in the file".
   - `Formula`: "RIP-Relative Difference".
   - `Symbol`: "global_var_1".
   - IF Loader picks `BSS=0x5000`, THEN `Byte 30` MUST BE `...`.
   - IF Loader picks `BSS=0x8000`, THEN `Byte 30` MUST BE `...`.

5. **Why call it a Constraint?**
   - It **constrains** the final binary code.
   - The code is not valid *unless* this rule is applied.
   - The Loader cannot just "load and run". It is *forced* to solve this equation for every single global variable access.

## Derivation 13: Anatomy of `R_X86_64_PC32 .bss+0x8`
1. **The Name Breakdown**:
   - `R`: Relocation (The "To-Do" Note).
   - `X86_64`: Architecture (64-bit Intel/AMD).
   - `PC32`: The Formula Type.
     - `PC`: Program Counter (synonym for RIP).
     - `32`: The size of the hole is 32-bits (4 bytes).
     - **Derivation of Formula**:
       1. **Goal**: Reach `TargetAddress` from `CurrentRIP`.
       2. **Vector Math**: $Start + Vector = End$.
       3. **Substitution**: $CurrentRIP + Value = TargetAddress$.
       4. **Solve for Value**: $Value = TargetAddress - CurrentRIP$.
       5. **Format**: Store `Value` as 32-bit signed integer.
     - **Resulting Name**: `PC32` (PC-Relative 32-bit).

2. **The Target Breakdown (`.bss+0x8`)**:
   - `.bss`: The Section Name.
     - Recall: Compiler groups all uninitialized variables into one block called `.bss`.
   - `+0x8`: The Offset **inside** that block.
     - Variable 1 is at Offset 0x0. (Size 8 bytes).
     - Variable 2 is at Offset 0x8. (Size 8 bytes).
     - Therefore: `.bss+0x8` uniquely identifies **Variable 2**.

3. **Combined Meaning**:
   - "Go to the patch location in the code."
   - "Calculate distance to the 2nd global variable (Offset 8)."
   - "Write the result as a 32-bit integer."
   - "This converts `MOV [0]` into `MOV [RIP - 1234]`."

4. **Why `0x8`?**:
   - Because `unsigned long` is 8 bytes.
   - 1st variable: Bytes 0-7.
   - 2nd variable: Bytes 8-15.
   - Start of 2nd variable is `0 + 8` = `0x8`.

## Derivation 14: The "Lookup Table" Mechanism (Symbol Resolution)
1. **The Question**: "How does the Loader know that `.bss+0x8` means *Variable 2*?"
2. **The Map**: The ELF file contains a **Symbol Table** (`.symtab`).
   - Row 1: `global_var_1` → Section `.bss`, Value `0x00`.
   - Row 2: `global_var_2` → Section `.bss`, Value `0x08`.
   - Row 3: `global_var_3` → Section `.bss`, Value `0x10`.

3. **The Link**:
   - The Relocation Entry actually stores an **Index** to the Symbol Table (not just the string ".bss+0x8").
   - Example Entry: `Offset=1e`, `Type=PC32`, `Symbol_Index=38` (Row 2).

4. **The Algorithm (Step-by-Step execution by Loader)**:
   - Step A: Loader reads Relocation Entry `1e`.
   - Step B: Loader sees `Symbol_Index = 38`.
   - Step C: Loader goes to `.symtab[38]`. Reads `Value = 0x08` (Offset relative to BSS).
   - Step D: Loader calculates `TargetAddress = AddressOfBSS + 0x08`.
   - Step E: Loader calculates `PatchValue = TargetAddress - CurrentRIP`.
   - Step F: Loader writes `PatchValue` into memory at `1e`.

## Derivation 15: Axiomatic Reconstruction (Full Numerical Trace)
01. | File Offset | Content (Hex)   | Symbol Index | Symbol Name    | Section Index | Section Offset | Meaning
02. |-------------|-----------------|--------------|----------------|---------------|----------------|--------
03. | 0x0000001B  | 48 C7 05        | -            | -              | .text         | 0x1B           | MOVQ Instruction Prefix
04. | 0x0000001E  | 00 00 00 00     | -            | -              | .text         | 0x1E           | **Placeholder Hole** (Length 4)
05. | RELOC_TBL   | 1E (Offset)     | 38 (.bss+8)  | global_var_2   | .rel.text     | -              | Constraint Rule
06. | SYMBOL_TBL  | 38              | global_var_2 | -              | .bss          | 0x00000008     | Target Relative Location

07. **Loader Step 1: Allocation (Random Input)**
08. $Loader_{input} \rightarrow RandomState \rightarrow A_{text} = 0xFFFF1000, A_{bss} = 0xFFFF5000$.
09. Constraint 1: $.text$ loaded at $0xFFFF1000$.
10. Constraint 2: $.bss$. loaded at $0xFFFF5000$.
11. $\therefore A_{global\_var\_2} = A_{bss} + Offset_{sym} = 0xFFFF5000 + 0x00000008 = 0xFFFF5008$.

12. **Loader Step 2: Instruction Pointer Calculation (Geometry)**
13. $Instruction_{start} = A_{text} + Offset_{insn} = 0xFFFF1000 + 0x1B = 0xFFFF101B$.
14. $Instruction_{len} = 7$ bytes (3 prefix + 4 hole).
15. $RIP_{next} = Instruction_{start} + Instruction_{len} = 0xFFFF101B + 7 = 0xFFFF1022$.

16. **Loader Step 3: Solving the Constraint (Algebra)**
17. Formula: $Value = Target - RIP$.
18. Input: $Target = 0xFFFF5008, RIP = 0xFFFF1022$.
19. Calc: $0xFFFF5008 - 0xFFFF1022$.
20. High Bits: $0xFFFF - 0xFFFF = 0x0000$.
21. Low Bits: $0x5008 - 0x1022$.
22. low bit calc: $8 - 2 = 6$.
23. low bit calc: $0 - 2 \rightarrow borrow \rightarrow 16 - 2 = E$.
24. low bit calc: $0 - 0 - 1 (borrow) \rightarrow borrow \rightarrow 16 - 1 = F$.
25. low bit calc: $5 - 1 - 1 (borrow) = 3$.
26. Result: $0x3FE6$.
27. Check: $0xFFFF1022 + 0x3FE6$.
    - $0x1022 + 0x3FE6$.
    - $2+6=8$.
    - $2+E=16 \rightarrow 0, carry 1$.
    - $0+F+1=16 \rightarrow 0, carry 1$.
    - $1+3+1=5$.
    - Result $0x5008$. ✓ Match.

28. **Loader Step 4: Patching (Memory Operation)**
29. Patch Address: $P = A_{text} + Offset_{hole} = 0xFFFF1000 + 0x1E = 0xFFFF101E$.
30. Action: `WRITE(0xFFFF101E, 0x00003FE6, Size=4)`.
31. Memory Before: `... 48 C7 05 00 00 00 00 ...`
32. Memory After:  `... 48 C7 05 E6 3F 00 00 ...` (Little Endian).

33. **CPU Execution Step (Runtime Verification)**
34. CPU @ $0xFFFF101B$ reads `48 C7 05 E6 3F 00 00`.
35. Decode: MOVQ, IP-Relative.
36. Execute: $EffectiveAddr = RIP_{current} + Length + Offset_{signed}$.
37. $EffectiveAddr = 0xFFFF101B + 7 + 0x3FE6 = 0xFFFF5008$.
38. Access $0xFFFF5008$.
39. Is $0xFFFF5008 == A_{global\_var\_2}$? Yes. ✓

## Derivation 16: The Relocation Story (Real Data Narrative)

**Chapter 1: The Compiler's Dilemma**
Imagine a builder (Compiler) writing a letter to a friend. He wants to say: "Meet me at the 2nd chair in the lounge."
But the builder is in a dark room (File). He doesn't know where the lounge is or where the chairs are. The room (Memory) hasn't been built yet.
So he writes: "Meet me at [BLANK]."
He sticks a blue post-it note (Relocation Entry) on the [BLANK] spot.
The note says: "Whoever places the chairs, please measure the distance from *this specific letter* to the *2nd Chair*, and write that number here."
He seals the envelope (The .ko File).

**Chapter 2: The Loader's Arrival**
The Operating System (Loader) is the architect. It opens the envelope.
"Okay, I need space for the letter (Code) and space for the chairs (Data/BSS)."
It looks at the vast empty field (RAM).
"I'll put the letter at address `0xFFFF1000`."
"I'll put the lounge at address `0xFFFF5000`."

**Chapter 3: The Calculation**
The Loader sees the blue post-it note.
"Ah, I need to fill in this blank at `0xFFFF101E`."
"Where is the 2nd Chair?"
"Well, the lounge is at `0xFFFF5000`. The 2nd chair is 8 feet (bytes) inside. So `0xFFFF5008`."
"Where is the reader standing when they read this?"
"They read the instruction, so their eyes (RIP) are at the end of the sentence: `0xFFFF1022`."
"Distance = Chair(`0xFFFF5008`) - Eyes(`0xFFFF1022`) = `0x3FE6`."

**Chapter 4: The Patch**
The Loader takes a pen.
It crosses out [BLANK] (which was 0x00).
It writes `0x3FE6` in the letter.
The letter now reads: "Meet me at [Current Position + 0x3FE6]."
The Loader peels off the post-it note (Discards Relocation Info).
The Code is now Live.

**Chapter 5: The Execution**
The CPU (Reader) picks up the letter at `0xFFFF1000`.
It reads line by line.
It arrives at `0xFFFF101E`.
"Move to... [Current+0x3FE6]."
Current is `0xFFFF1022`.
Math: `0xFFFF1022 + 0x3FE6 = 0xFFFF5008`.
The CPU walks exactly to `0xFFFF5008`.
It finds the 2nd Chair.
Success.

## Derivation 17: Separation of Concerns (Who knows what?)

1. **The Writer (Compiler)**:
   - Action: `emit_byte(0x48)` (Offset 0).
   - Action: `emit_byte(0xC7)` (Offset 1).
   - Action: `emit_byte(0x05)` (Offset 2).
   - **Critical Moment**: Needs to emit Address. Logic: "I don't know it."
   - Action: `mark_relocation(current_offset)`. -> Stores `3` in list.
   - Action: `emit_byte(0x00)` (Offset 3).
   - Action: `emit_byte(0x00)` (Offset 4).
   - Action: `emit_byte(0x00)` (Offset 5).
   - Action: `emit_byte(0x00)` (Offset 6).

2. **The Resulting File (.ko)**:
   - [.init.text Section]: `48 C7 05 00 00 00 00`
   - [.rela.init.text Section]: `Entry { Offset: 3, Info: ... }`

3. **The Reader (Loader)**:
   - Input 1: The Code Block.
   - Input 2: The Relocation List.
   - Action: Loop through List.
   - Item 1: "Offset 3".
   - Loader logic: `Address = CodeBase + 3`.
   - Loader does **NOT** look at bytes 0, 1, 2. It jumps straight to 3.
   - Loader writes value. Done.

4. **Conclusion**:
   - The Compiler **calculates** the offset `3` because it is generating the bytes.
   - The Loader **consumes** the offset `3` blindly.

## Derivation 18: Why specificially "-4"?
1. **The Formula**: $Value = Target - RIP$.
2. **The Components**:
   - $P$ = Address of the Patch (Where we write `00 00 00 00`).
   - $RIP$ = Address of the **Next Instruction** (Where CPU starts executing after this).
3. **The Geometry**:
   - Instruction Start: $S$.
   - Opcode: Bytes $S, S+1, S+2$.
   - Patch Start ($P$): Byte $S+3$.
   - Patch Bytes: $S+3, S+4, S+5, S+6$. (Total 4 bytes).
   - Instruction End: $S+7$.
   - $RIP$ (Next Instruction): Points to $S+7$.

4. **Relation between P and RIP**:
   - $P$ starts at $S+3$.
   - $RIP$ is at $S+7$.
   - Distance: $RIP - P = (S+7) - (S+3) = 4$.
   - $\therefore RIP = P + 4$.

5. **Substitute into Formula**:
   - $Value = Target - (P + 4)$.
   - $Value = Target - P - 4$.

## Derivation 19: The Sign Flip (Why Minus?)
1. **The Law of Vectors**:
   - To find the vector FROM $A$ TO $B$, you calculate $B - A$.
   - Here, Start Point = $RIP$.
   - End Point = $Data$.
   - Vector = $Data - RIP$.

2. **The Definition of RIP**:
   - $RIP$ is always "Current Position + 4". (Assuming simple encoding).
   - $RIP = P + 4$.

3. **Substitution**:
   - Vector = $Data - ( P + 4 )$.
   - Vector = $Data - P - 4$.

4. **Example**:
   - Data is at 500. Patch ($P$) is at 100.
   - Hole is 4 bytes (100, 101, 102, 103).
   - RIP is at 104.
   - Vector = $500 - 104 = 396$.
   - Math Check: $500 - (100 + 4) = 396$.
   - Wrong Math ($Data - (P-4)$): $500 - 96 = 404$.
     - If hardware added 404 to RIP(104), result = 508. (Overshot by 8).
     - **Result**: Minus is correct.

## Derivation 20: Visual Geometry of PC-Relative Addressing

RAM (Linear Memory)
---------------------------------------------------------
100  [ Opcode ]
101  [ Opcode ]
102  [ Opcode ]
103  [ OFFSET ] <--- P (Patch Start)
104  [ OFFSET ]
105  [ OFFSET ]
106  [ OFFSET ]
107  [ Next   ] <--- RIP (Hardware Reference Point)
...
...
500  [ DATA   ] <--- Target

Vector Calculation:
-------------------
We need: Vector from RIP (107) to Target (500).
Math: 500 - 107 = 393.
We write "393" into bytes 103-106.

Relocation Logic (Why P+4?):
----------------------------
P = 103.
P + 4 = 107.
This exactly matches RIP.
So: Target - (P + 4) = 500 - 107 = 393. ✓

## Derivation 21: The "Assembly Train" Analogy
Imagine a train (The Code Section).
Each car is an Instruction.
The Compiler is the Factory building the cars.

**Car Type A: "Small Load" (`global_val = 0`)**
- Requires: 3 feet for Engine + 4 feet for Address + 4 feet for Cargo.
- Total Length: 11 feet.
- The "Address Hole" is at foot 3.
- The "Next Car" starts at foot 11.
- Distance from Hole to Next Car = 11 - 3 = **8 feet**.

**Car Type B: "Read Only" (`temp = global_val`)**
- Requires: 3 feet for Engine + 4 feet for Address.
- Total Length: 7 feet.
- The "Address Hole" is at foot 3.
- The "Next Car" starts at foot 7.
- Distance from Hole to Next Car = 7 - 3 = **4 feet**.

**The Pattern:**
- Since the loop (`i = 0, 1, 2, 3, 4`) does the *same thing* 5 times, it builds 5 identical **Type A** cars.
- Each car is 11 feet long.
- Each car has a hole.
- Each hole is 8 feet from the back of its own car.
