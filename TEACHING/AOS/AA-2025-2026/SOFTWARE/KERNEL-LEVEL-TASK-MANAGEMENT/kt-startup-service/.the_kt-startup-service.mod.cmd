savedcmd_the_kt-startup-service.mod := printf '%s\n'   kt-startup-service.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_kt-startup-service.mod
