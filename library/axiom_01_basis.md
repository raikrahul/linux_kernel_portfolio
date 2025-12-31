# Axiom 01: The Machine Basis

## 1. Counting (The Integer)
The machine knows only relationships of count.
*   **AXIOM**: A count is a distinct value in a sequence (0, 1, 2...).
*   **AXIOM**: Addition is the combination of counts.

## 2. The Bit (The Physical Switch)
*   **AXIOM**: A physical wire has a voltage state.
    *   High Voltage = 1
    *   Low Voltage = 0
*   **DERIVATION**: We can represent specific integers using a sequence of bits (Binary).

## 3. The Byte (Addressable Unit)
*   **AXIOM**: The x86_64 architecture groups 8 Bits into 1 Byte.
*   **CALCULATION**:
    ```text
    2^8 = 256 states
    Range: [0, 255] (0x00 to 0xFF)
    ```
*   **IMPLICATION**: The Byte is the smallest unit of **Memory Addressing**. You cannot address a specific bit directly; you must address the byte containing it.

## 4. Memory (The Array)
*   **DEFINITION**: RAM is a linear array of Bytes.
*   **AXIOM**: Each Byte has a unique Index called an **Address**.
*   **OBSERVATION** (Live System):
    *   This machine is x86_64.
    *   Pointer Size = 64 bits (8 Bytes).
    *   Max Addressable Space = 2^64 bytes (theoretical).

## 5. The Page (The Management Unit)
*   **AXIOM**: Managing Memory byte-by-byte is inefficient for the hardware (MMU).
*   **DERIVATION**: Hardware groups bytes into fixed-size blocks called **Pages**.
*   **OBSERVATION**: `getconf PAGE_SIZE` = 4096 bytes.
*   **CALCULATION**:
    ```text
    4096 = 2^12
    Offset bits = 12 (Bits 0-11)
    ```
