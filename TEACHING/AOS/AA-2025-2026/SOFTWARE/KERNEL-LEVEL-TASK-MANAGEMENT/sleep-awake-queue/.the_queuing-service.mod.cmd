savedcmd_the_queuing-service.mod := printf '%s\n'   queuing-service.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_queuing-service.mod
