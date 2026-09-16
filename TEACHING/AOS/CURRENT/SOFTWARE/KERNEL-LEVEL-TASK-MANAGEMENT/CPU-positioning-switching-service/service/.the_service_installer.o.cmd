savedcmd_the_service_installer.o := ld -m elf_x86_64 -z noexecstack --no-warn-rwx-segments   -r -o the_service_installer.o @the_service_installer.mod 
