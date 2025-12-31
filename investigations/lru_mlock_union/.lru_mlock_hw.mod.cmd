savedcmd_lru_mlock_hw.mod := printf '%s\n'   lru_mlock_hw.o | awk '!x[$$0]++ { print("./"$$0) }' > lru_mlock_hw.mod
