savedcmd_proc_file_manage.mod := printf '%s\n'   proc_file_manage.o | awk '!x[$$0]++ { print("./"$$0) }' > proc_file_manage.mod
