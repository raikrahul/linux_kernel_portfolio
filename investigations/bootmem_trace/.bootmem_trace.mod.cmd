savedcmd_bootmem_trace.mod := printf '%s\n'   bootmem_trace.o | awk '!x[$$0]++ { print("./"$$0) }' > bootmem_trace.mod
