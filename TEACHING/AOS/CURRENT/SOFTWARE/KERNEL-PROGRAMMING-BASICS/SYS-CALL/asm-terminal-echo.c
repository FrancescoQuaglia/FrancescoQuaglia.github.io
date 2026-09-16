/* please compile with 'gcc -c' and 'ld' or with '-nostartfiles' */


void instring(char *s);
void outstring(char *s);
char c; //putting c here (low addresses) allows correct runs when using ecx as pointer to c with -D m32 compilation

char* newline="\n";
char v[1024];


void instring(char*);
void outstring(char*);

int _start(int argc){

 while(1){
  instring(v);
  outstring(v);
  outstring(newline);
 }

}

void instring(char *s){
 long  i = 0;

 do{
#ifndef m32
   asm("mov %%rbx, %%rdi ; mov %%rcx , %%rsi ; syscall " :  : "a" (0) , "b" (0) , "c" ((unsigned long)(&c)) , "d" ((long)(1)) );
#else
    asm("int $0x80" :  : "a" (3) , "b" (0) , "c" ((unsigned long)(&c)) , "d" ((long)(1)) ); 
#endif
   s[i] = c;
   i++; 
 }
 while ( (c!='\n') && (c!=' ') && (c!='\t'));
 i--;
 s[i]='\0';
}

void outstring(char *s){
 long  i=0;
 while (s[i++]!='\0') ;
 i--;
#ifndef m32
 asm("mov %%rbx, %%rdi ; mov %%rcx , %%rsi ; syscall " :  : "a" (1) , "b" (1) , "c" ((unsigned long)(s)) , "d" ((long)(i)) );
#else
 asm("int $0x80" :  : "a" (4) , "b" (1) , "c" ((unsigned long)(s)) , "d" ((long)(i)) );
#endif
}

