savedcmd_page_labs.mod := printf '%s\n'   page_labs.o | awk '!x[$$0]++ { print("./"$$0) }' > page_labs.mod
