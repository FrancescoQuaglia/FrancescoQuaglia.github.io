savedcmd_the_work_queues.mod := printf '%s\n'   work-queues.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_work_queues.mod
