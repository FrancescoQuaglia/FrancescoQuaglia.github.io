//please compile with "gcc -c -fomit-frame-pointer" to achieve the below listed outcome from objdump

int f(int x){
	if (x == 1) return 1;
	return 0;
}

/****************************************
Disassembly of section .text:

0000000000000000 <f>:
   0:   89 7c 24 fc             mov    %edi,-0x4(%rsp)
   4:   83 7c 24 fc 01          cmpl   $0x1,-0x4(%rsp)
   9:   75 07                   jne    12 <f+0x12>
   b:   b8 01 00 00 00          mov    $0x1,%eax
  10:   eb 05                   jmp    17 <f+0x17>
  12:   b8 00 00 00 00          mov    $0x0,%eax
  17:   c3                      retq 
****************************************/
