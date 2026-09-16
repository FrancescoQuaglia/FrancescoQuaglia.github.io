savedcmd_singlefilefs.mod := printf '%s\n'   singlefilefs_src.o file.o dir.o | awk '!x[$$0]++ { print("./"$$0) }' > singlefilefs.mod
