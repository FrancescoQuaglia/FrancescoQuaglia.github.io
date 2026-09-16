savedcmd_idt_entry_discovery.mod := printf '%s\n'   idt_entry_discovery.o | awk '!x[$$0]++ { print("./"$$0) }' > idt_entry_discovery.mod
