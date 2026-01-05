# TLB / CR3 DEMO - TWO PROCESSES, SAME VA, DIFFERENT PA

## EXPECTED RESULT

```
PROCESS A                           PROCESS B
─────────────────────────────       ─────────────────────────────
PID = 1234                          PID = 5678
buffer VA = 0x55A1B2C3D000          buffer VA = 0x55A1B2C3D000  ← SAME VA!
buffer[0] = 'A'                     buffer[0] = 'B'             ← DIFFERENT DATA!

KERNEL MODULE OUTPUT:               KERNEL MODULE OUTPUT:
CR3 = 0x11E273000                   CR3 = 0x24FB27000           ← DIFFERENT CR3!
...                                 ...
PT[x] = 0x...123...                 PT[x] = 0x...456...         ← DIFFERENT PTE!
PA = 0x123456000                    PA = 0x789ABC000            ← DIFFERENT PA!
```

## WHY THIS HAPPENS

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              RAM (PHYSICAL)                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  0x11E273000: ┌─────────────┐    0x24FB27000: ┌─────────────┐              │
│               │ PROCESS A   │                 │ PROCESS B   │              │
│               │ PML4 TABLE  │                 │ PML4 TABLE  │              │
│               └──────┬──────┘                 └──────┬──────┘              │
│                      │                               │                      │
│                      ↓                               ↓                      │
│  0x393C01000: ┌─────────────┐    0x4A5B06000: ┌─────────────┐              │
│               │ PROCESS A   │                 │ PROCESS B   │              │
│               │ PDPT TABLE  │                 │ PDPT TABLE  │              │
│               └──────┬──────┘                 └──────┬──────┘              │
│                      │                               │                      │
│                      ↓                               ↓                      │
│                     ...                             ...                     │
│                      │                               │                      │
│                      ↓                               ↓                      │
│  0x123456000: ┌─────────────┐    0x789ABC000: ┌─────────────┐              │
│               │ AAAAAAAAAA  │                 │ BBBBBBBBBB  │              │
│               │ (4KB page)  │                 │ (4KB page)  │              │
│               └─────────────┘                 └─────────────┘              │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

                      ↑                               ↑
                      │                               │
        SAME VA: 0x55A1B2C3D000           SAME VA: 0x55A1B2C3D000
        
        BUT CR3 IS DIFFERENT → DIFFERENT PAGE TABLES → DIFFERENT PA!
```

## TLB SCENARIO

```
TIME 0: Process A running, CR3 = 0x11E273000
        CPU accesses VA 0x55A1B2C3D000
        TLB MISS → Walk tables → Find PA 0x123456000
        TLB stores: {VA=0x55A1B2C3D, PA=0x123456, ASID=1}
        Read RAM[0x123456000] = 'A' ✓

TIME 1: Context switch to Process B
        CR3 changes to 0x24FB27000
        YOUR AMD CPU: TLB FLUSHED (no PCID)
        TLB is now EMPTY

TIME 2: Process B running, CR3 = 0x24FB27000
        CPU accesses VA 0x55A1B2C3D000 (SAME VA!)
        TLB MISS (was flushed) → Walk B's tables → Find PA 0x789ABC000
        TLB stores: {VA=0x55A1B2C3D, PA=0x789ABC, ASID=2}
        Read RAM[0x789ABC000] = 'B' ✓

WITHOUT TLB FLUSH: Would read 'A' instead of 'B' → SECURITY BUG!
```

## RUN THE DEMO

```bash
# Terminal 1: Compile and run Process A
cd /home/r/Desktop/lecpp/kernel_module_raw
gcc -o process_a process_a.c
./process_a
# Note the VA, keep running

# Terminal 2: Compile and run Process B  
cd /home/r/Desktop/lecpp/kernel_module_raw
gcc -o process_b process_b.c
./process_b
# Note the VA (may be same or similar!)

# Terminal 3: Load kernel module and walk both addresses
cd /home/r/Desktop/lecpp/kernel_module
sudo insmod pagewalk_driver.ko

# Walk Process A's address (copy from Terminal 1 output)
echo '0x<VA_FROM_A>' | sudo tee /proc/pagewalk
cat /proc/pagewalk
# Note: CR3 = ???, PA = ???

# Walk Process B's address (copy from Terminal 2 output)
echo '0x<VA_FROM_B>' | sudo tee /proc/pagewalk
cat /proc/pagewalk
# Note: CR3 = DIFFERENT!, PA = DIFFERENT!
```
