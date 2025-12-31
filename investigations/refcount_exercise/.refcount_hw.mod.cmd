savedcmd_refcount_hw.mod := printf '%s\n'   refcount_hw.o | awk '!x[$$0]++ { print("./"$$0) }' > refcount_hw.mod
