# THE COMPLETE AXIOMATIC GUIDE TO RELOCATION

## Real Data Source
- Module: `pagewalk_driver.ko`
- Module: `gap_test.ko`
- Module: `reloc_test.ko`
- Kernel: `6.14.0-37-generic`
- Compiler: `gcc-13`

---

# PART 1: PRIMITIVES

## Axiom 1: The Byte
```
01. Unit = 1 byte.
02. 1 byte = 8 bits.
03. 1 byte = value 0x00 to 0xFF.
04. 1 byte = 1 memory cell.
```

## Axiom 2: The Address
```
05. Address = number identifying memory cell.
06. Address 0x1000 = cell at position 4096.
07. Address + 1 = adjacent cell.
08. Range: 0x0000000000000000 to 0xFFFFFFFFFFFFFFFF (64-bit).
```

## Axiom 3: The Sequence
```
09. Sequence = multiple bytes in order.
10. Sequence [A, B, C]: Position 0 = A, Position 1 = B, Position 2 = C.
11. Length = count of bytes.
```

## Axiom 4: The File
```
12. File = sequence of bytes on disk.
13. File offset = position in file.
14. File offset 0 = first byte.
15. File offset N = (N+1)th byte.
```

## Axiom 5: The Section
```
16. Section = named region of file.
17. .text = code bytes (instructions).
18. .data = initialized variables.
19. .bss = uninitialized variables (size only, no bytes on disk).
20. .symtab = symbol table (variable/function list).
21. .rela.text = relocation entries for .text section.
```

---

# PART 2: THE CPU

## Axiom 6: The Register
```
22. Register = storage inside CPU.
23. RAX, RBX, RCX, RDX = general purpose.
24. RIP = Instruction Pointer.
25. RIP holds address of NEXT instruction.
```

## Axiom 7: The Instruction
```
26. Instruction = bytes telling CPU what to do.
27. Part 1: OPCODE (what operation).
28. Part 2: OPERAND (data for operation).
29. Length: 1 to 15 bytes (variable).
```

## Axiom 8: The Execution Cycle
```
30. Cycle repeats:
31.   A. Fetch bytes from address in RIP.
32.   B. Decode bytes into instruction.
33.   C. Advance RIP by instruction length.
34.   D. Execute instruction.
35. Key: RIP updated BEFORE execution.
36. ∴ During execution, RIP = address of NEXT instruction.
```

## Axiom 9: RIP-Relative Addressing
```
37. Some instructions access memory.
38. Address calculated: Target = RIP + Offset.
39. Offset = 32-bit signed integer in instruction.
40. This is "Position Independent Code" (PIC).
```

---

# PART 3: THE PROBLEM

## Axiom 10: The Time Gap
```
41. Compile Time: Compiler writes file to disk.
42. Load Time: Loader copies file to RAM.
43. Run Time: CPU executes instructions.
44. Compile Time << Load Time << Run Time.
45. Addresses decided at Load Time.
46. Compiler runs at Compile Time.
47. ∴ Compiler cannot know addresses.
```

## Axiom 11: The Address Uncertainty
```
48. Variable "x" will be in RAM.
49. Where in RAM? Unknown at Compile Time.
50. Instruction needs to access "x".
51. Instruction needs address of "x".
52. Compiler cannot provide address.
53. ∴ Instruction is incomplete.
```

---

# PART 4: THE SOLUTION

## Axiom 12: The Placeholder
```
54. Compiler writes temporary value: 0x00000000.
55. This is obviously wrong.
56. Loader will fix it later.
```

## Axiom 13: The Relocation Entry
```
57. Compiler writes a NOTE in the file.
58. Note location: .rela section.
59. Note format:
    - OFFSET: file position of placeholder.
    - SYMBOL: which variable to reach.
    - TYPE: formula to calculate value.
    - ADDEND: correction constant.
```

## Axiom 14: The Symbol Table
```
60. Compiler tracks all variables.
61. Each variable gets a row in .symtab.
62. Row format: (Name, Section, Offset, Size).
63. Example: (x, .bss, 0, 8) = "x is in .bss at offset 0, size 8 bytes".
64. Relocation entry references row by index.
```

---

# PART 5: THE LOADER

## Axiom 15: Memory Allocation
```
65. Loader decides where to put each section.
66. .text → address A (e.g., 0x1000).
67. .bss → address B (e.g., 0x5000).
68. Addresses now KNOWN.
```

## Axiom 16: Code Copying
```
69. Loader copies .text bytes to RAM at A.
70. Placeholders are copied too (still 0x00000000).
```

## Axiom 17: BSS Zeroing
```
71. .bss has no bytes on disk.
72. Loader allocates RAM for .bss size.
73. Loader writes 0x00 to all .bss bytes.
74. Variables now exist (initialized to 0).
```

## Axiom 18: Relocation Processing
```
75. Loader reads .rela section.
76. For each entry:
77.   a. Read OFFSET, SYMBOL, TYPE, ADDEND.
78.   b. Calculate patch address: P = A + OFFSET.
79.   c. Look up symbol in .symtab.
80.   d. Calculate symbol address: S = (section base) + (symbol offset).
81.   e. Apply formula: Value = S + ADDEND - P.
82.   f. Write Value to address P.
```

---

# PART 6: THE FORMULA

## Axiom 19: Why Subtraction?
```
83. CPU calculates: Target = RIP + Offset.
84. We want: Target = S (symbol address).
85. ∴ Offset = S - RIP.
86. Offset = S - RIP.
```

## Axiom 20: RIP vs P
```
87. P = address where Offset is stored.
88. RIP = address of NEXT instruction.
89. Offset field is 4 bytes.
90. RIP = P + 4.
```

## Axiom 21: Deriving the Addend
```
91. Offset = S - RIP.
92. RIP = P + 4.
93. ∴ Offset = S - (P + 4).
94. Offset = S - P - 4.
95. Offset = (S - P) + (-4).
96. Standard form: Offset = S + A - P, where A = -4.
97. ADDEND = -4.
```

## Axiom 22: Why -4?
```
98. OFFSET FIELD = 4 bytes.
99. RIP = end of OFFSET FIELD.
100. RIP = P + 4.
101. Gap = 4 bytes.
102. Addend = -Gap = -4.
```

## Axiom 23: Different Addends
```
103. Some instructions have data AFTER the offset.
104. Example: MOV [RIP+disp], immediate.
105. Layout: [Opcode 3B][Disp 4B][Imm 4B] = 11 bytes.
106. Disp starts at byte 3.
107. Instruction ends at byte 11.
108. RIP = byte 11.
109. P = byte 3.
110. Gap = 11 - 3 = 8.
111. Addend = -8.
```

---

# PART 7: REAL DATA FROM pagewalk_driver.o

## Assembly Evidence
```
  f7:   48 89 35 00 00 00 00    mov    %rsi,0x0(%rip)        # fe
                        fa: R_X86_64_PC32       .bss-0x4
```

## Analysis
```
112. Instruction starts at 0xF7.
113. Bytes: 48 89 35 00 00 00 00.
114. Length = 7 bytes.
115. OPCODE: 48 89 35 (3 bytes).
116. OFFSET FIELD: 00 00 00 00 (4 bytes at 0xFA).
117. Next instruction: 0xFE.
118. P = 0xFA (where offset is stored).
119. RIP = 0xFE.
120. Gap = 0xFE - 0xFA = 4.
121. Addend = -4 ✓.
```

---

# PART 8: REAL DATA FROM gap_test.o

## Assembly Evidence (Read Operation)
```
  1d:   48 8b 05 00 00 00 00    mov    0x0(%rip),%rax        # 24
                        20: R_X86_64_PC32       .bss-0x4
```

## Analysis
```
122. Instruction starts at 0x1D.
123. Length = 7 bytes.
124. OFFSET at 0x20.
125. Next instruction at 0x24.
126. Gap = 0x24 - 0x20 = 4.
127. Addend = -4 ✓.
```

## Assembly Evidence (Write Immediate Operation)
```
  24:   48 c7 05 00 00 00 00    movq   $0x12345678,0x0(%rip)        # 2f
  2b:   78 56 34 12 
                        27: R_X86_64_PC32       .bss-0x8
```

## Analysis
```
128. Instruction starts at 0x24.
129. Bytes: 48 c7 05 [4B disp] [4B imm].
130. Length = 11 bytes (3 + 4 + 4).
131. OFFSET at 0x27.
132. Next instruction at 0x2F.
133. Gap = 0x2F - 0x27 = 8.
134. Addend = -8 ✓.
```

---

# PART 9: NUMERICAL TRACE

## Setup
```
135. Code loaded at 0x1000.
136. BSS loaded at 0x5000.
137. Variable "x" at BSS + 0 = 0x5000.
138. Instruction at code offset 0x1D.
139. OFFSET FIELD at code offset 0x20.
```

## Calculation
```
140. P = 0x1000 + 0x20 = 0x1020.
141. S = 0x5000.
142. A = -4.
143. Value = S + A - P.
144. Value = 0x5000 + (-4) - 0x1020.
145. Step: 0x5000 - 4 = 0x4FFC.
146. Step: 0x4FFC - 0x1020:
147.   C - 0 = C.
148.   F - 2 = D.
149.   F - 0 = F.
150.   4 - 1 = 3.
151. Value = 0x3FDC.
```

## Verification
```
152. RIP = P + 4 = 0x1020 + 4 = 0x1024.
153. Target = RIP + Value = 0x1024 + 0x3FDC.
154.   4 + C = 0x10 → 0, carry 1.
155.   2 + D + 1 = 0x10 → 0, carry 1.
156.   0 + F + 1 = 0x10 → 0, carry 1.
157.   1 + 3 + 1 = 5.
158. Target = 0x5000 ✓.
```

---

# PART 10: USER MISTAKES

## Mistake 1: "Who does 1e - 1b = 3?"
```
Problem: User thought machine calculates delta.
Reality: Human observer calculates for understanding.
Compiler: Tracks cursor position, stores "1e" directly.
Loader: Reads "1e" from file, never calculates delta.
Prevention: Separate human analysis from machine execution.
```

## Mistake 2: "But there is no 3"
```
Problem: Looking for explicit "3" in bytes.
Reality: "3" is structural (byte count), not a stored value.
The "3" exists as: cursor position when placeholder starts.
Prevention: Understand implicit vs explicit data.
```

## Mistake 3: "Why minus (P+4)?"
```
Problem: Confused about subtraction vs addition.
Reality: Distance = Destination - Start.
Vector math: To go FROM A TO B, compute B - A.
RIP is at (P+4), we subtract it from Target.
Prevention: Draw number line, apply vector rule.
```

## Mistake 4: "Is instruction always 7 bytes?"
```
Problem: Generalized from one example.
Reality: x86-64 instructions vary 1-15 bytes.
Evidence: MOV [RIP], imm32 = 11 bytes.
Prevention: Never assume constancy in CISC.
```

## Mistake 5: "Why need offset when we know value?"
```
Problem: Confused value (data) with address (location).
Reality: We know WHAT to write (0x12345678).
We don't know WHERE to write it.
Offset provides the WHERE.
Prevention: Separate data from address.
```

## Mistake 6: "Who told loader x is at 0 in .bss?"
```
Problem: Missing link between relocation and symbol table.
Reality: Compiler writes symbol table entry.
Entry says: (x, .bss, offset=0, size=8).
Relocation entry stores symbol INDEX, not name.
Loader looks up index in symbol table.
Prevention: Trace the data chain through all tables.
```

## Mistake 7: "Compiler knows only 3 (cursor)"
```
Problem: Thought addend comes from cursor.
Reality: Cursor → OFFSET field (where to patch).
Instruction format → ADDEND (gap to RIP).
Separate sources for separate fields.
Prevention: Map each field to its origin.
```

---

# PART 11: ASCII DIAGRAMS

## Instruction Layout (7 bytes, addend = -4)
```
Address:  0x1000  0x1001  0x1002  0x1003  0x1004  0x1005  0x1006  0x1007
          +-------+-------+-------+-------+-------+-------+-------+-------+
Bytes:    |  48   |  8B   |  05   |  00   |  00   |  00   |  00   | NEXT  |
          +-------+-------+-------+-------+-------+-------+-------+-------+
          |     OPCODE            |     OFFSET (placeholder)      |  RIP→ |
                                  ↑                               ↑
                                  P = 0x1003                      RIP = 0x1007
                                  
Gap = RIP - P = 0x1007 - 0x1003 = 4
Addend = -4
```

## Instruction Layout (11 bytes, addend = -8)
```
Address:  0x1000 ... 0x1002  0x1003  0x1004  0x1005  0x1006  0x1007  0x1008  0x1009  0x100A  0x100B
          +------+---+------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
Bytes:    |  48  |C7 |  05  |  00   |  00   |  00   |  00   |  78   |  56   |  34   |  12   | NEXT  |
          +------+---+------+-------+-------+-------+-------+-------+-------+-------+-------+-------+
          |     OPCODE      |     OFFSET (placeholder)      |     IMMEDIATE DATA      |  RIP→ |
                            ↑                                                         ↑
                            P = 0x1003                                                RIP = 0x100B

Gap = RIP - P = 0x100B - 0x1003 = 8
Addend = -8
```

## File Structure
```
+------------------+
|    ELF Header    |
+------------------+
|     .text        | ← Code with placeholders
+------------------+
|     .data        | ← Initialized data
+------------------+
|     .bss         | ← (Size only, no bytes)
+------------------+
|    .symtab       | ← Symbol table
+------------------+
|   .rela.text     | ← Relocation entries
+------------------+
```

## Symbol Table
```
+-------+---------------+---------+--------+------+
| Index | Name          | Section | Offset | Size |
+-------+---------------+---------+--------+------+
|   1   | target_vaddr  | .bss    |   0    |  8   |
|   2   | proc_write    | .text   |  0x10  | 200  |
|   3   | proc_read     | .text   | 0x150  | 800  |
+-------+---------------+---------+--------+------+
```

## Relocation Table
```
+--------+-------------+------------+--------+
| Offset | SymbolIndex | Type       | Addend |
+--------+-------------+------------+--------+
|  0xFA  |      1      | R_X86_PC32 |   -4   |
|  0x27  |      1      | R_X86_PC32 |   -8   |
+--------+-------------+------------+--------+
```

---

# PART 12: LOADER ALGORITHM

```
function process_relocations():
    for each entry in .rela.text:
        # Step 1: Read entry fields
        offset = entry.offset
        symbol_index = entry.symbol
        type = entry.type
        addend = entry.addend
        
        # Step 2: Calculate patch address
        P = text_base + offset
        
        # Step 3: Look up symbol
        symbol = symtab[symbol_index]
        section_base = get_section_base(symbol.section)
        S = section_base + symbol.offset
        
        # Step 4: Apply formula
        if type == R_X86_64_PC32:
            value = S + addend - P
        
        # Step 5: Write value
        write_32bit_le(P, value)
```

---

# PART 13: VERIFICATION CHECKLIST

```
□ Instruction bytes match expected encoding
□ Placeholder is at expected offset
□ Relocation entry offset matches placeholder position
□ Symbol table entry exists for target variable
□ Addend matches instruction structure (-4 or -8)
□ After patching: RIP + Value = Symbol Address
```

---

# PART 14: SUMMARY TABLE

| Concept | Definition |
|---------|------------|
| Byte | 8 bits, value 0-255 |
| Address | Number identifying memory cell |
| Instruction | Bytes telling CPU what to do |
| Opcode | Part of instruction: the operation |
| Operand | Part of instruction: the data |
| RIP | Register holding next instruction address |
| Placeholder | Temporary 0x00000000 for unknown address |
| Relocation | Process of fixing placeholders |
| Symbol Table | List of variables with section/offset |
| Addend | Correction for gap between P and RIP |

---

# PART 15: FORMULAS

| Formula | Meaning |
|---------|---------|
| RIP = P + Gap | RIP is Gap bytes after patch location |
| Target = RIP + Offset | CPU calculates target address |
| Offset = Target - RIP | Required offset value |
| Offset = S - (P + Gap) | Substitute RIP |
| Offset = S + (-Gap) - P | Standard form |
| Value = S + A - P | Loader formula (A = -Gap) |

---

# END OF DOCUMENT
