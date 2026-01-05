# x86-64 Assembly Tutorial for C++ Programmers

## SOURCES

This tutorial consolidates assembly concepts from 55+ files in `src/` and `docs/`:
- 16 `.s` files (real assembly output)
- 35 `.md` files (derivations and lessons)
- Key files: `72_assembly_axioms.md`, `72_vptr_vtable_lesson_plan.md`, `78_visit_generic_lambda.cpp`

---

## PART 1: REGISTERS (30 AXIOMS)

### 1.1 General Purpose Registers

| Register | Bits | Subregisters | Role |
|----------|------|--------------|------|
| `%rax` | 64 | `%eax` (32), `%ax` (16), `%al` (8) | Return value |
| `%rbx` | 64 | `%ebx`, `%bx`, `%bl` | Callee-saved |
| `%rcx` | 64 | `%ecx`, `%cx`, `%cl` | 4th argument |
| `%rdx` | 64 | `%edx`, `%dx`, `%dl` | 3rd argument |
| `%rsi` | 64 | `%esi`, `%si`, `%sil` | 2nd argument |
| `%rdi` | 64 | `%edi`, `%di`, `%dil` | 1st argument / `this` |
| `%rbp` | 64 | `%ebp` | Base pointer |
| `%rsp` | 64 | `%esp` | Stack pointer |
| `%r8`-`%r15` | 64 | `%r8d`-`%r15d` (32) | Extra registers |

From `72_assembly_axioms.md` STEP 01:
```
RAX = 0x123456789ABCDEF0
EAX = 0x9ABCDEF0 (lower 32 bits)
AX  = 0xDEF0 (lower 16 bits)
AL  = 0xF0 (lower 8 bits)
```

**TRAP:** Writing to `%eax` zeros bits [63..32] of `%rax`.

### 1.2 XMM Registers (Floating Point)

| Register | Bits | Role |
|----------|------|------|
| `%xmm0` | 128 | 1st FP argument, FP return |
| `%xmm1`-`%xmm7` | 128 | FP arguments 2-8 |

From `61_constexpr_roundoff_errors.s`:
```asm
movss   .LC4(%rip), %xmm0    ; load float from .rodata
mulss   %xmm1, %xmm0         ; multiply floats
addss   %xmm1, %xmm2         ; add floats
cvtss2sd %xmm1, %xmm0        ; float → double
```

---

## PART 2: INSTRUCTION SET

### 2.1 Data Movement

| Instruction | Size | Meaning |
|-------------|------|---------|
| `movq %src, %dst` | 8 bytes | Copy 64-bit |
| `movl %src, %dst` | 4 bytes | Copy 32-bit |
| `movw %src, %dst` | 2 bytes | Copy 16-bit |
| `movb %src, %dst` | 1 byte | Copy 8-bit |
| `movzbl (%src), %dst` | 1→4 | Load byte, zero-extend |
| `movsd (%src), %xmm` | 8 bytes | Load double to XMM |
| `movss (%src), %xmm` | 4 bytes | Load float to XMM |

From `72_assembly_axioms.md` STEP 04:
```
q = quad = 8 bytes
l = long = 4 bytes
w = word = 2 bytes
b = byte = 1 byte
```

### 2.2 Address Computation

| Instruction | Meaning |
|-------------|---------|
| `leaq -48(%rbp), %rax` | `%rax = %rbp - 48` (address only) |
| `movq -48(%rbp), %rax` | `%rax = *(rbp - 48)` (value) |

From `72_assembly_axioms.md` STEP 07:
```
leaq -48(%rbp), %rax
  %rbp = 0x7FFF0100
  %rax = 0x7FFF0100 - 48 = 0x7FFF00D0
  NO memory read happens.
```

**TRAP:** `leaq` calculates address, `movq` dereferences.

### 2.3 Arithmetic

| Instruction | Meaning |
|-------------|---------|
| `addq $8, %rax` | `%rax += 8` |
| `subq $32, %rsp` | `%rsp -= 32` (allocate stack) |
| `imulq $2, %rax, %rax` | `%rax = %rax × 2` |
| `salq $3, %rax` | `%rax <<= 3` (multiply by 8) |

From `72_assembly_axioms.md` STEP 17:
```
salq $3, %rax  ; RAX = 5 → 40 (5 × 8)
; Used for array indexing: addr = base + index × sizeof(element)
```

### 2.4 Stack Operations

| Instruction | Effect |
|-------------|--------|
| `pushq %rbp` | `%rsp -= 8; *(%rsp) = %rbp` |
| `popq %rbp` | `%rbp = *(%rsp); %rsp += 8` |
| `call label` | Push return address, jump |
| `ret` | Pop return address, jump |

From `72_assembly_axioms.md` STEP 11-14:
```
pushq %rbp
  RSP = 0x7FFF00C8 → new RSP = 0x7FFF00C0
  memory[0x7FFF00C0] = RBP

ret
  RIP = memory[RSP]
  RSP += 8
```

### 2.5 Comparison & Jumps

| Instruction | Meaning |
|-------------|---------|
| `cmpq %rax, %rbx` | Set flags based on `%rbx - %rax` |
| `testb %al, %al` | Set ZF=1 if `%al == 0` |
| `je label` | Jump if ZF=1 (equal) |
| `jne label` | Jump if ZF=0 (not equal) |
| `jb label` | Jump if CF=1 (below, unsigned) |
| `jmpq *%rdx` | Indirect jump to address in `%rdx` |

---

## PART 3: MEMORY ADDRESSING MODES

| Mode | Syntax | Meaning |
|------|--------|---------|
| Immediate | `$42` | Literal value 42 |
| Register | `%rax` | Value in register |
| Direct | `label` | Value at label address |
| Indirect | `(%rax)` | Value at address in `%rax` |
| Displacement | `-8(%rbp)` | Value at `%rbp - 8` |
| Indexed | `(%rax,%rcx,8)` | Value at `%rax + %rcx×8` |

From `44_assembly_virtual_dispatch.s`:
```asm
movq    (%rax), %rax        ; indirect: load vptr
movq    (%rax), %rdx        ; indirect: load vtable[0]
call    *%rdx               ; indirect jump
```

From `78_visit_generic_lambda.cpp`:
```asm
movq (%rcx,%rax,8), %rdx    ; indexed: dispatch_table[index]
; address = %rcx + %rax × 8
; %rcx = table base, %rax = index, 8 = pointer size
```

---

## PART 4: FUNCTION CALLS (System V AMD64 ABI)

### 4.1 Argument Passing

| Argument | Integer | Floating Point |
|----------|---------|----------------|
| 1st | `%rdi` | `%xmm0` |
| 2nd | `%rsi` | `%xmm1` |
| 3rd | `%rdx` | `%xmm2` |
| 4th | `%rcx` | `%xmm3` |
| 5th | `%r8` | `%xmm4` |
| 6th | `%r9` | `%xmm5` |
| 7th+ | Stack | Stack |

From `72_assembly_axioms.md` STEP 21:
```
call foo(a, b, c)
  %rdi = a (1st arg)
  %rsi = b (2nd arg)
  %rdx = c (3rd arg)
```

### 4.2 Stack Frame

```
High Address
┌─────────────────────────┐
│ Caller's frame          │
├─────────────────────────┤
│ Return address (8B)     │ ← pushed by call
├─────────────────────────┤
│ Saved %rbp (8B)         │ ← pushq %rbp
├─────────────────────────┤ ← %rbp points here
│ Local variables         │ ← subq $N, %rsp
├─────────────────────────┤
│ Spilled registers       │
└─────────────────────────┘ ← %rsp points here
Low Address
```

From `75_geometry_intersection.s`:
```asm
pushq   %r15
pushq   %r14
pushq   %r13
pushq   %r12
pushq   %rbp
pushq   %rbx
subq    $104, %rsp          ; 104 bytes for locals
movsd   %xmm0, (%rsp)       ; spill 'a'
movsd   %xmm1, 8(%rsp)      ; spill 'b'
```

---

## PART 5: VIRTUAL DISPATCH (vptr/vtable)

### 5.1 Object Layout

```
Object @ 0xA000:
┌─────────────────────────┐
│ vptr (8B) → vtable      │ offset 0
├─────────────────────────┤
│ member1 (4B)            │ offset 8
├─────────────────────────┤
│ member2 (4B)            │ offset 12
└─────────────────────────┘
sizeof = 16 bytes
```

From `72_assembly_axioms.md` STEP 29:
```
sizeof = 16
&obj = 0xA000
&obj.weight = 0xA008 → offset = 8 (vptr occupies 0-7)
&obj.age = 0xA00C → offset = 12
```

### 5.2 Vtable Layout

```
vtable @ 0x404000:
┌─────────────────────────┐
│ offset-to-top (8B) = 0  │ offset -16
├─────────────────────────┤
│ RTTI pointer (8B)       │ offset -8
├─────────────────────────┤
│ slot[0] = &speak        │ offset 0  ← vptr points here
├─────────────────────────┤
│ slot[1] = &destructor   │ offset 8
└─────────────────────────┘
```

From `72_assembly_axioms.md` STEP 30:
```asm
leaq 16+_ZTV17AnimalBaseVirtual(%rip), %rdx
; %rdx = vtable base + 16 = &slot[0]
; +16 skips offset-to-top (8B) + RTTI (8B)
```

### 5.3 Virtual Call Sequence

From `44_assembly_virtual_dispatch.s` TODO BLOCK 5:
```asm
leaq    -24(%rbp), %rax     ; step 1: object address
movq    (%rax), %rax        ; step 2: load vptr
movq    (%rax), %rdx        ; step 3: load vtable[0]
leaq    -24(%rbp), %rdi     ; step 4: this pointer
call    *%rdx               ; step 5: indirect call
```

From `72_vptr_vtable_lesson_plan.md`:
```
Direct call:   call _ZN6Animal3fooEi      ; address in instruction
Indirect call: call *(%rax)               ; address from memory
```

---

## PART 6: VARIANT DISPATCH (std::visit)

### 6.1 Variant Layout

```
std::variant<Circle, Rectangle, Ellipse> @ 0x3000:
┌─────────────────────────┐
│ index (1B) = 0x00       │ offset 0
├─────────────────────────┤
│ padding (7B)            │ offset 1-7
├─────────────────────────┤
│ union storage (32B)     │ offset 8-39
└─────────────────────────┘
sizeof = 40 bytes
```

From `76_variant_alias_step0.cpp`:
```
sizeof(Circle)    = 24 bytes (3 × double)
sizeof(Rectangle) = 32 bytes (4 × double)
sizeof(Ellipse)   = 32 bytes (4 × double)

Union size = max(24, 32, 32) = 32 bytes
Header = 1 (index) + 7 (padding) = 8 bytes
Total = 8 + 32 = 40 bytes
```

### 6.2 Dispatch Table

```
dispatch_table @ 0x23A0:
┌─────────────────────────┐
│ [0] = 0x2100 (Circle)   │
├─────────────────────────┤
│ [1] = 0x2200 (Rectangle)│
├─────────────────────────┤
│ [2] = 0x2300 (Ellipse)  │
└─────────────────────────┘
```

From `78_visit_generic_lambda.cpp`:
```asm
movzbl  (%rsi), %eax            ; load variant.index()
leaq    dispatch_table(%rip), %rcx
movq    (%rcx,%rax,8), %rdx     ; handler = table[index]
addq    $8, %rsi                ; skip tag, point to union
jmpq    *%rdx                   ; jump to handler
```

### 6.3 Multi-Variant (2D Table)

From `80_visit_multi_variant.cpp`:
```
variant<A,B> × variant<X,Y> = 2×2 = 4 handlers

Flat index = v1.index() × 2 + v2.index()

index 0: (A, X)
index 1: (A, Y)
index 2: (B, X)
index 3: (B, Y)
```

---

## PART 7: LOOP UNROLLING

### 7.1 Runtime Loop Overhead

From `65_unroll_dot_product.md`:
```asm
.L10:
    movl (%rsi,%rax,4), %edx      ; load offsets[k]
    addl %r8d, %edx               ; idx = i + offset
    movslq %edx, %rdx             ; sign extend
    movsd (%rdi,%rdx,8), %xmm0    ; load u[idx]
    mulsd (%rcx,%rax,8), %xmm0    ; multiply by weights[k]
    addq $1, %rax                 ; k++           ← OVERHEAD
    addsd %xmm0, %xmm1            ; sum += ...
    cmpq %r9, %rax                ; k < size?     ← OVERHEAD
    jb .L10                       ; jump back     ← OVERHEAD
```

Overhead per iteration: 3 instructions (addq, cmpq, jb)
N=3 iterations: 9 wasted instructions

### 7.2 Template Unrolling Result

From `54_recursive_template_asm.s`:
```asm
main:
    movl    $6, %eax    ; result = 1+2+3 computed at compile time!
    ret
```

Entire loop replaced by constant.

---

## PART 8: CONSTEXPR vs RUNTIME

### 8.1 Compile-Time Computation

From `61_constexpr_roundoff_errors.s`:
```asm
movss   .LC7(%rip), %xmm0    ; load pre-computed constexpr result
; .LC7 contains the final value, computed by compiler
```

### 8.2 Runtime Computation

```asm
.L5:
    mulss   %xmm1, %xmm0     ; multiply
    addl    $1, %eax         ; loop counter
    cmpl    %eax, %edx
    jne     .L5              ; loop
```

---

## PART 9: EXERCISES

### E1: Register Subsets
```
RAX = 0xDEADBEEF12345678
What is: EAX? AX? AL?
```
Answer: EAX=0x12345678, AX=0x5678, AL=0x78

### E2: Virtual Call Sequence
Given object at 0xA000 with vptr → vtable at 0x404010.
Trace: `ptr->foo()` where foo is slot[0].
```
1. movq (%rdi), %rax      ; %rax = ?
2. call *(%rax)           ; jumps to ?
```
Answer: %rax = 0x404010, jumps to *(0x404010)

### E3: Variant Dispatch Index
```
variant<int, double> v1 = 3.14;    // index = ?
variant<string, double> v2 = 2.5;  // index = ?
Flat index for 2D dispatch = ?
```
Answer: v1.index()=1, v2.index()=1, flat = 1×2+1 = 3

### E4: Stack Allocation
```asm
pushq   %rbp
movq    %rsp, %rbp
subq    $48, %rsp
```
If initial RSP = 0x7FFF0100, what is final RSP?
Answer: 0x7FFF0100 - 8 (push) - 48 (sub) = 0x7FFF00B0

### E5: leaq vs movq
```asm
leaq    8(%rdi), %rax    ; A
movq    8(%rdi), %rax    ; B
```
If %rdi = 0x1000 and memory[0x1008] = 42:
A: %rax = ?
B: %rax = ?

Answer: A: 0x1008 (address), B: 42 (value)

---

## FILE REFERENCES

| Topic | Key Files |
|-------|-----------|
| Axioms | [72_assembly_axioms.md](file:///home/r/Desktop/lecpp/src/72_assembly_axioms.md) |
| vptr/vtable | [72_vptr_vtable_lesson_plan.md](file:///home/r/Desktop/lecpp/src/72_vptr_vtable_lesson_plan.md) |
| Virtual dispatch | [44_assembly_virtual_dispatch.s](file:///home/r/Desktop/lecpp/src/44_assembly_virtual_dispatch.s) |
| No virtual | [42_no_virtual_assembly.s](file:///home/r/Desktop/lecpp/src/42_no_virtual_assembly.s) |
| Variant visit | [78_visit_generic_lambda.cpp](file:///home/r/Desktop/lecpp/src/78_visit_generic_lambda.cpp) |
| Multi-variant | [80_visit_multi_variant.cpp](file:///home/r/Desktop/lecpp/src/80_visit_multi_variant.cpp) |
| Loop unroll | [65_unroll_dot_product.md](file:///home/r/Desktop/lecpp/src/65_unroll_dot_product.md) |
| Constexpr | [61_constexpr_roundoff_errors.s](file:///home/r/Desktop/lecpp/src/61_constexpr_roundoff_errors.s) |
| Stencil | [64_unrolled_finite_difference.md](file:///home/r/Desktop/lecpp/src/64_unrolled_finite_difference.md) |
