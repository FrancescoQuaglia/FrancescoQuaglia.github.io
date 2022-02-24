#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUFFER 1000

int error_function() {
   printf("Syntax: analyzer [-p] <nome_file>\n");
   exit(-1);
}


int main (int argc, char *argv[]){


 int fd, res_r, index;
 char buffer[MAX_BUFFER];
 char *filename;
 char nextchar;


 int continous = 0;
int temp_charnum=0, temp_linenum=0, temp_wordnum=0;
 int charnum=0,linenum=0, wordnum=0;
 int newword = 0;

if ((argc<2) || (argc>3)) error_function();
if ((argc== 2) && (argv[1][0] == '-')) error_function();
if ((argc== 3) && (strcmp(argv[1], "-p")!=0)) error_function();
if ((argc== 3) && (strcmp(argv[1], "-p")==0)) continous= 1;
if (argc== 2) filename=argv[1];
else filename =argv[2];
fd= open(filename, O_RDONLY| O_EXCL);
if (fd== -1) {perror("Open error: ");exit(-2); }
do {
   res_r = 1;
   temp_linenum=0; temp_wordnum=0; temp_charnum=0;
   newword=1;
   while (res_r){
      res_r = read (fd,buffer,MAX_BUFFER);
      index=0;
         while(index <res_r) {
         temp_charnum++;
            if ((buffer[index] != '\n') &&(buffer[index] != ' ')){
            if (newword) {newword=0; temp_wordnum++; }
             } else newword=1;
          if (buffer[index] == '\n') temp_linenum++;
        index++;
  }
}

if ((linenum!= temp_linenum) || (wordnum!= temp_wordnum) ||(charnum!= temp_charnum)) {
   linenum = temp_linenum; wordnum = temp_wordnum;charnum = temp_charnum;
   printf("Number of characters: %d; number of words:%d; number of lines:%d\n",
   charnum,wordnum,linenum);
   }
   if (continous) {sleep(1); lseek(fd, 0,SEEK_SET);}
} while(continous);
exit(0);
}

