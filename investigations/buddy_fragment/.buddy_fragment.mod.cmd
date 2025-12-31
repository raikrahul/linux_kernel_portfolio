savedcmd_buddy_fragment.mod := printf '%s\n'   buddy_fragment.o | awk '!x[$$0]++ { print("./"$$0) }' > buddy_fragment.mod
