savedcmd_flags_zone_node.mod := printf '%s\n'   flags_zone_node.o | awk '!x[$$0]++ { print("./"$$0) }' > flags_zone_node.mod
