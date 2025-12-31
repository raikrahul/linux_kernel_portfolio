savedcmd_fork_trace.mod := printf '%s\n'   fork_trace.o | awk '!x[$$0]++ { print("./"$$0) }' > fork_trace.mod
