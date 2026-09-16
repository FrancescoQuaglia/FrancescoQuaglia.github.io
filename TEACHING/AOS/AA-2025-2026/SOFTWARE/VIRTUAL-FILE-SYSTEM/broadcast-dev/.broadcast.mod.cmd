savedcmd_broadcast.mod := printf '%s\n'   broadcast.o | awk '!x[$$0]++ { print("./"$$0) }' > broadcast.mod
