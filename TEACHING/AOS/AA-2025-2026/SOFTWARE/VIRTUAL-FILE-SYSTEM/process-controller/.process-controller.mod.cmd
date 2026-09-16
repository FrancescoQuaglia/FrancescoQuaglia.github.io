savedcmd_process-controller.mod := printf '%s\n'   process-controller.o | awk '!x[$$0]++ { print("./"$$0) }' > process-controller.mod
