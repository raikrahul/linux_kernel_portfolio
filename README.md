# Linux Kernel Investigations
> *Deriving the machine from first principles.*

## The Philosophy: Axiomatic Kernel Hacking

This repository contains a collection of rigorous investigations into the Linux Kernel 6.14+.
The goal is not just to "learn" the kernel, but to **derive** it.

### The Rules of Engagement
1.  **No Magic**: Every concept must be traced back to a source file, a specialized hardware register, or a mathematical axiom.
2.  **Primate Coding**: We assume nothing but counting, binary logic, and C basics. We build up from there.
3.  **Grill Everything**: Do not accept "it just works." Ask **why** it works, **where** it lives in RAM, and **how** the CPU sees it.
4.  **Data Over Dogma**: Values are verified with live data traces (`/proc`, `dmesg`, `bpftrace`) from a running machine.

## The Library

### [Investigations](./investigations/)
Deep dives into specific kernel subsystems.
- **Page Table Locks**: Deriving spinlock contention from the ground up.
- **Memory Mapping**: Decoding the `struct page` mapping field bit-by-bit.

### [Axioms](./library/)
The fundamental truths we have established.
- **Counting & Bits**: The base units of the machine.
- **Memory**: Physical vs Virtual, PFNs, and Page Tables.

### [Developer Diary](./dev_diary/)
Raw, socratic dialogues documenting the struggle to understand.
*Warning: Contains unfiltered frustration, wrong turns, and the eventual light of understanding.*

---
*Maintained by [The Primate Coder]. Built on an experimental Linux x86_64 machine.*
