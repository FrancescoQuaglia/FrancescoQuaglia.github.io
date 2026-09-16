savedcmd_sys-example.mod := printf '%s\n'   sys-example.o | awk '!x[$$0]++ { print("./"$$0) }' > sys-example.mod
