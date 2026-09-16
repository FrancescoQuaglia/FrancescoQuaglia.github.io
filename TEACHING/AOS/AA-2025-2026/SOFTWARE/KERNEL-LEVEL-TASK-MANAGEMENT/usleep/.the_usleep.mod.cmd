savedcmd_the_usleep.mod := printf '%s\n'   usleep.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_usleep.mod
