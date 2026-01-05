
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

/*
 * VARIABLE ANALYSIS: global_val
 * =================================================================================================
 *
 * MEMORY STATE (BSS):
 * -------------------
 * [Offset 0x00] global_val (8 bytes) -> Value: 0x0000000000000000.
 *
 * NUMERICAL GEOMETRY:
 * 1. Size = 8 bytes (64-bit).
 * 2. Alignment = 8 bytes.
 * 3. Section = .bss (NOBITS).
 *
 * W-QUESTIONS:
 * - What: 8 bytes of storage for uninitialized integer.
 * - Where: .bss section index 28 (from readelf).
 * - Why: To test 2 different Instruction Geometries targeting THIS same address.
 */
static volatile unsigned long global_val;

static int __init gap_test_init(void) {
    /*
     * CASE 1: READ OPERATION (The "Short Gap")
     * =============================================================================================
     * SOURCE LINE: unsigned long temp = global_val;
     *
     * ASSEMBLY ANALYSIS (Start at 0x16):
     * ----------------------------------
     * Byte Stream: 48 8B 05 00 00 00 00
     * Indices:     [0] [1] [2] [3] [4] [5] [6]
     * Components:
     *   - 0,1,2: 48 8B 05 (MOV RAX, [RIP+Disp32]) -> 3 bytes opcode.
     *   - 3,4,5,6: 00 00 00 00 (Displacement Hole) -> 4 bytes.
     *   - END: Index 7 (Start of Next Instruction).
     *
     * GEOMETRY CALCULATION:
     * 1. P (Patch Start) = Address(0x16) + 3 = 0x19.
     * 2. RIP (Next Insn) = Address(0x16) + 7 = 0x1D.
     * 3. Gap (RIP - P)   = 0x1D - 0x19 = 4 bytes.
     * 4. Formula: Value = Target - RIP.
     * 5. Substitution: Value = Target - (P + 4).
     * 6. Addend: -4.
     *
     * RELOCATION ENTRY:
     * - Offset: 0x19.
     * - Type: R_X86_64_PC32.
     * - Symbol: .bss
     * - Addend: -4.
     *
     * EXECUTION TRACE (N=1):
     * - RIP=0x1D.
     * - Target=0x5000 (.bss).
     * - Value = 0x5000 - 0x1D = 0x4FE3.
     * - Memory at 0x19 becomes: E3 4F 00 00.
     * - CPU calculates: 0x1D + 0x4FE3 = 0x5000. ✓
     *
     * EXECUTION TRACE (Harder N=0xFFFFFF):
     * - RIP=0xFFFFFF.
     * - Target=0x2000000.
     * - Value = 0x2000000 - 0xFFFFFF = 0x1000001.
     * - Check: 0xFFFFFF + 0x1000001 = 0x2000000. ✓
     *
     * W-QUESTIONS:
     * - What: 7-byte instruction reading memory.
     * - Why: Shortest encoding for RIP-relative load.
     * - Where: .init.text+0x16.
     * - Without: If not -4, pointer points to garbage.
     */
    unsigned long temp = global_val;

    /*
     * CASE 2: WRITE IMMEDIATE (The "Long Gap")
     * =============================================================================================
     * SOURCE LINE: global_val = 0x12345678;
     *
     * ASSEMBLY ANALYSIS (Start at 0x1F):
     * ----------------------------------
     * Byte Stream: 48 C7 05 00 00 00 00 78 56 34 12
     * Indices:     [0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10]
     * Components:
     *   - 0,1,2: 48 C7 05 (MOVQ [RIP+Disp32], Imm32) -> 3 bytes opcode.
     *   - 3,4,5,6: 00 00 00 00 (Displacement Hole) -> 4 bytes.
     *   - 7,8,9,10: 78 56 34 12 (Intermediate Data) -> 4 bytes.
     *   - END: Index 11 (Start of Next Instruction 0x2A).
     *
     * GEOMETRY CALCULATION:
     * 1. P (Patch Start) = Address(0x1F) + 3 = 0x22.
     * 2. RIP (Next Insn) = Address(0x1F) + 11 = 0x2A.
     * 3. Gap (RIP - P)   = 0x2A - 0x22 = 8 bytes.  <--- SURPRISE! 8, not 4.
     * 4. Formula: Value = Target - RIP.
     * 5. Substitution: Value = Target - (P + 8).
     * 6. Addend: -8.
     *
     * RELOCATION ENTRY:
     * - Offset: 0x22.
     * - Type: R_X86_64_PC32.
     * - Symbol: .bss
     * - Addend: -8. (Implicit in calculation).
     *
     * EXECUTION TRACE (N=1):
     * - RIP=0x2A.
     * - Target=0x5000 (.bss).
     * - Value = 0x5000 - 0x2A = 0x4FD6.
     * - Memory at 0x22 becomes: D6 4F 00 00.
     * - CPU calculates: 0x2A + 0x4FD6 = 0x5000. ✓
     *
     * PUNISHMENT CALCULATION (Edge Case: Wrap Around):
     * - RIP = 0xFFFFFFFF (4GB boundary).
     * - Target = 0x00000008 (Low memory).
     * - Math: 0x8 - 0xFFFFFFFF = 0x9. (Overflows 64-bit? No, 32-bit signed offset?).
     * - Wait: 0x8 - (-1) = 9.
     * - Check: 0xFFFFFFFF + 9.
     * - F...F + 1 = 0.
     * - 0 + 8 = 8. ✓
     *
     * W-QUESTIONS:
     * - What: 11-byte instruction writing constant.
     * - Why: Needs 4 bytes for address AND 4 bytes for value (0x12..78).
     * - Where: .init.text+0x1F.
     * - Which: Using -8 logic because Offset is NOT at the end of instruction.
     */
    global_val = 0x12345678;

    /*
     * INTERIM ACTION: Function Call
     * =============================
     * This pushes code apart.
     */
    printk(KERN_INFO "Gap Test Running\n");

    /*
     * CASE 3: WRITE IMMEDIATE LARGE (64-bit)
     * ======================================
     * SOURCE LINE: global_val = 0x1223122344567;
     * This requires a DIFFERENT instruction because 64-bit immediate
     * cannot fit in standard MOV [mem], imm32.
     * Compiler might use: MOV RAX, imm64 -> MOV [RIP+Off], RAX.
     */
    /*
     * CASE 4: LOOP ASSIGNMENT (Repetition)
     * ====================================
     * SOURCE: for(i=0; i<5; i++) global_val = i;
     *
     * HYPOTHESIS:
     * - If compiled as a loop: 1 MOV instruction inside the loop body.
     *   -> 1 Relocation Entry.
     * - If unrolled: 5 MOV instructions.
     *   -> 5 Relocation Entries.
     */
    int i;
    for (i = 0; i < 5; i++) {
        global_val = i;
    }

    return 0;
}

static void __exit gap_test_exit(void) {}

module_init(gap_test_init);
module_exit(gap_test_exit);
MODULE_LICENSE("GPL");
