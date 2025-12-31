savedcmd_anon_file_hw.mod := printf '%s\n'   anon_file_hw.o | awk '!x[$$0]++ { print("./"$$0) }' > anon_file_hw.mod
