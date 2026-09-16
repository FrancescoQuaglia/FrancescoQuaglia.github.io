savedcmd_the_tasklets.mod := printf '%s\n'   tasklets.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_tasklets.mod
