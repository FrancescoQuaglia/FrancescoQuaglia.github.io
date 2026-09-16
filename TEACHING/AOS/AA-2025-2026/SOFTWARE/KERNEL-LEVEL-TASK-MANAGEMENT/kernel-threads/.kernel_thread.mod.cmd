savedcmd_kernel_thread.mod := printf '%s\n'   kernel_thread.o | awk '!x[$$0]++ { print("./"$$0) }' > kernel_thread.mod
