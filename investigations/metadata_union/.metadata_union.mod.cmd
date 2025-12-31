savedcmd_metadata_union.mod := printf '%s\n'   metadata_union.o | awk '!x[$$0]++ { print("./"$$0) }' > metadata_union.mod
