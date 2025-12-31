# Log 001: The Axiomatic Inception

**Date**: 2025-12-31
**Context**: Project Initialization

## The Problem
We have a directory full of kernel exercises (`/learnlinux`), but no coherent story. They are scattered worksheets, C files, and raw notes. We know the material, but we cannot show it.

## The Design Decision
**Brutalist Technical**.
Most portfolios are fluff. They hide the code behind flashy CSS. We decided to go the opposite direction:
1.  **Text First**: The content is the aesthetic.
2.  **No Magic**: Every number must be derived.
3.  **Axiomatic**: We don't say "The kernel does X". We say "Given Axiom A (Hardware) and Axiom B (Code), the kernel MUST do X".

## The Inventory
We identified 13 core investigations to migrate:
*   Foundations (Bootmem, Buddy, NUMA)
*   Page Mechanics (Struct Page, Flags, Unions)
*   Advanced Topics (Split Locks, Mapping Decode)

## The Transformation
Each "Worksheet" (a messy scratchpad) is being transformed into a "Derivation" (a polished proof).
*   *Worksheet*: "I think this is 4KB."
*   *Derivation*: "PAGE_SIZE is defined as 4096 by `getconf`. Therefore..."

## Next Steps
We are currently deploying this to GitHub Pages. The challenge now is maintaining the integrity of the links and ensuring the code compiles for the reader.
