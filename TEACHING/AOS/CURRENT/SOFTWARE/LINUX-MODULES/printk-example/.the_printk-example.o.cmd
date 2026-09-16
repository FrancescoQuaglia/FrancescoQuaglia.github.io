savedcmd_the_printk-example.o := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments   -r -o the_printk-example.o @the_printk-example.mod 
