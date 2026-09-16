savedcmd_the_tasklets.o := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments   -r -o the_tasklets.o @the_tasklets.mod 
