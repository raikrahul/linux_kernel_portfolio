# Axiom 02: The Duality of Context

**Subject**: Time, Interrupts, and The Scheduler
**Derived From**: `gfp_context_bug`

---

## 1. The Fundamental State
At any given nanosecond, a CPU Core is in exactly one of two states:
1.  **Task Context**: Executing a thread of instructions on behalf of a Process (System or User).
2.  **Interrupt Context**: Executing an Event Handler triggered by Hardware.

## 2. Derivation: The Asymmetry

### Task Context (The Standard)
*   **Definition**: A state where `current` points to a valid `task_struct`.
*   **Property**: It has a stack. It has a state (Run, Sleep, Stop).
*   **Implication**: It CAN be scheduled out. If it needs data from disk, it yields the CPU.

### Interrupt Context (The Exception)
*   **Definition**: A state injected by hardware (Timer, Network Card).
*   **Property**: It steals the CPU from the current task. It borrows the stack of the victim task (or uses a dedicated IRQ stack).
*   **Implication**: It CANNOT be scheduled out.
    *   *Why?* Who would it yield to? It *is* the interruption. If it sleeps, the interrupted task never resumes. The system freezes.

## 3. The Rules of Time
*   **Axiom**: You cannot wait for an event (Sleep) if you are the event handler.
*   **Consequence**: `alloc_page(GFP_KERNEL)` is illegal in Interrupt Context because it implies waiting for swap. You must use `alloc_page(GFP_ATOMIC)`.
