savedcmd_phys_proof.mod := printf '%s\n'   phys_proof.o | awk '!x[$$0]++ { print("./"$$0) }' > phys_proof.mod
