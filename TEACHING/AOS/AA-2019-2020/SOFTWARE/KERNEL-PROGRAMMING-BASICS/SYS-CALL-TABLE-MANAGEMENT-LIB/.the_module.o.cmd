cmd_/home/francesco/kernel-modules/syscall-table-management-lib/the_module.o := ld -m elf_x86_64  -z max-page-size=0x200000   -r -o /home/francesco/kernel-modules/syscall-table-management-lib/the_module.o /home/francesco/kernel-modules/syscall-table-management-lib/example-usage.o /home/francesco/kernel-modules/syscall-table-management-lib/lib/scth.o ; scripts/mod/modpost /home/francesco/kernel-modules/syscall-table-management-lib/the_module.o