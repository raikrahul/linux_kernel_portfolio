savedcmd_mapping_hw.mod := printf '%s\n'   mapping_hw.o | awk '!x[$$0]++ { print("./"$$0) }' > mapping_hw.mod
