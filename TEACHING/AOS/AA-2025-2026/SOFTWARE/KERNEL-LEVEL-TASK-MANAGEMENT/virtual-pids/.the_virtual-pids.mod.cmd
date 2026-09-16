savedcmd_the_virtual-pids.mod := printf '%s\n'   virtual-pids.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_virtual-pids.mod
