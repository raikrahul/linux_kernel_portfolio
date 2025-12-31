# Design System: The Axiomatic Standard

## Aesthetic: Brutalist Technical
*   **No Fluff**: Zero adjectives. Zero narrative filler.
*   **High Contrast**: Black and white logic.
*   **Text-First**: The content is the interface.

## Formatting Standards

### 1. The Derivation Block
Every article must follow the derivation pattern. Never state a fact without its origin.
```markdown
**AXIOM**: [Fundamental Truth]
**OBSERVATION**: [Live System Data]
**DERIVATION**: [Logical Step]
**CONCLUSION**: [Result]
```

### 2. Code & Data
*   **Variables**: Always use backticks. Example: `task_struct`.
*   **Memory Addresses**: Always use full 64-bit hex or explicit offsets.
*   **Live Data**: Must be cited with the source command.
    ```text
    SOURCE: /proc/meminfo
    VALUE: MemTotal: 16300000 kB
    ```

### 3. Visuals (ASCII)
We do not use images. We use text.
*   **Memory Maps**: Box drawing characters for layouts.
    ```text
    ┌─────────────┐ 0x0000
    │   HEADER    │
    ├─────────────┤ 0x0020
    │    DATA     │
    └─────────────┘
    ```
*   **Pointers**: Arrows using unicode. `ptr ──► target`

## File Structure
*   **Filename**: `snake_case.md`
*   **Title**: `# Title Case`
*   **Metadata**: Top of file block referencing original investigation date.
