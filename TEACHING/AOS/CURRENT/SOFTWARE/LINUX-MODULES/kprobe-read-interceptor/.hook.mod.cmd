savedcmd_hook.mod := printf '%s\n'   hook.o | awk '!x[$$0]++ { print("./"$$0) }' > hook.mod
