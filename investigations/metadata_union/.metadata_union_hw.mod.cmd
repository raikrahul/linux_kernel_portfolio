savedcmd_metadata_union_hw.mod := printf '%s\n'   metadata_union_hw.o | awk '!x[$$0]++ { print("./"$$0) }' > metadata_union_hw.mod
