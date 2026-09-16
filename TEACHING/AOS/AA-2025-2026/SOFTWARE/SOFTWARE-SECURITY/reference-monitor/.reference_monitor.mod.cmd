savedcmd_reference_monitor.mod := printf '%s\n'   reference_monitor.o | awk '!x[$$0]++ { print("./"$$0) }' > reference_monitor.mod
