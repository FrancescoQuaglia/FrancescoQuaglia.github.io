savedcmd_the_printk-example.mod := printf '%s\n'   printk-example.o lib/scth.o | awk '!x[$$0]++ { print("./"$$0) }' > the_printk-example.mod
