# ERROR REPORT: KERNEL ASSEMBLY & MEMORY FUNDAMENTALS

## MISTAKE 1: Assembly Expectation vs Reality (BSS Variables)
- **Line/Concept**: Expecting specific assembly instructions for .
- **What Went Wrong**: Assumed initialization of a static variable to zero creates explicit  instructions or data bytes in the assembly output.
- **Reality**: Zero-initialized variables go to  (NOBITS). They occupy NO space in the file and have NO instructions.
- **Why Sloppy**: Failed to distinguish between *imperative code* (instructions) and *declarative metadata* (section headers).
- **Prevention**: Use  and  to inspect variable storage, not .

## MISTAKE 2: Confusion on "Takes Zero Space"
- **Line/Concept**: "if it takes zero space then how does it even get loaded".
- **What Went Wrong**: Conflated *disk usage* (file size) with *runtime behavior* (process of loading).
- **Reality**: The ELF header acts as a reservation request. The "load" action is actually an "allocation" action by the kernel loader.
- **Why Sloppy**: Missed the distinction between the *artifact* (.ko file) and the *process* (loading).
- **Prevention**: Axiom: "Binaries are blueprints, Processes are houses." Blueprints just request space; they don't contain the empty space.

## MISTAKE 3: Relocation Mechanics
- **Line/Concept**: "what is the use of relocation then".
- **What Went Wrong**: Failed to connect the instruction stream (Code) to the variable location (Data).
- **Reality**: Code and Data are separate sections. Their relative distance is unknown at compile time. Relocation bridges this gap at load time via binary patching.
- **Why Sloppy**: Ignored the "Position Independent Code" (PIC) requirement of kernel modules.
- **Prevention**: Trace the lifecycle: Compile (local offset) -> Link (section offset) -> Load (physical address).

## MISTAKE 4: "Leak" Misclassification
- **Line/Concept**: "this seems a leak".
- **What Went Wrong**: Used user-space "memory leak" definition (lost allocation) for a kernel "dangling pointer" scenario.
- **Reality**: It is a Use-After-Free / Dangling Pointer. The memory *is* freed by the kernel. The pointer *remains* valid as a value but dangerous to dereference.
- **Why Sloppy**: Imprecise terminology. Leaks = Memory not freed. Dangling Ptrs = Memory freed but pointer kept.
- **Prevention**: Strict definitions. Ask: "Is the page returned to the free pool?" Yes -> Not a leak.

## MISTAKE 5: Kernel Memory Allocator Confusion
- **Line/Concept**: "but there should be no kmalloc in this case".
- **What Went Wrong**: Assumed  vs  distinction mattered for the *concept* of allocation.
- **Reality**: While strictly  (module loader), the fundamental operation is still "OS allocates pages".
- **Why Sloppy**: Got distracted by specific allocator API instead of the general mechanism (loader requesting pages).
- **Prevention**: Abstract to "Loader Allocation" first, then refine to specific API.
