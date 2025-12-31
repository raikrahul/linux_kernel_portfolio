savedcmd_compound_lab.mod := printf '%s\n'   compound_lab.o | awk '!x[$$0]++ { print("./"$$0) }' > compound_lab.mod
