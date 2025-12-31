savedcmd_flags_lab.mod := printf '%s\n'   flags_lab.o | awk '!x[$$0]++ { print("./"$$0) }' > flags_lab.mod
