savedcmd_the_service_installer.mod := printf '%s\n'   service_installer.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_service_installer.mod
