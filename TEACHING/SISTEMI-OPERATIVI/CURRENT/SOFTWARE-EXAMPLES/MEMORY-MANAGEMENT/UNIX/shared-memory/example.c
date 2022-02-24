#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define PAGE (4096)
#define NPAGES (10)
#define SIZE PAGE*NPAGES

#define DISP 128
#define OK DISP-1 

char messaggio[DISP];



void produttore(int id) {

   char *addr, *addr1;
   int ret;
	
   addr = shmat(id, 0 , SHM_W);

   if (addr == (void*)-1 ){
	printf("shmat error \n");	
	exit(-2);
   }

   addr1 = addr;

   printf("insert strings to write to shared memory ('quit' to close): \n");

   do{
     scanf("%127s", messaggio);
     memcpy(addr1, messaggio, DISP);
     addr1 = addr1 + DISP;
   }
   while( (strcmp(messaggio,"quit") != 0) && ((addr1 - addr) < (SIZE - DISP)));
   
   printf("shared memory update done\n");

   exit(0);
}


void consumatore(int id) {

   char *addr, *addr1;
   int ret;
   struct sembuf oper;

   addr = shmat(id, 0 , SHM_R);
   addr1 = addr; 
   

   printf("shared memory keeps the following strings: \n");

    while( (strcmp(addr1,"quit") != 0) && ((addr1 - addr) < (SIZE - DISP)))
    {
     printf("%s \n", addr1);
     addr1 = addr1 + DISP;
    }
   exit(0);

}



int main(int argc, char *argv[]) {

   long id_shm, id_sem;
   int  ret, STATUS;
   long key = 16;
   void *point;


   id_shm = shmget(key, SIZE, IPC_CREAT|0666);

   if (id_shm == -1){
	printf("shmget error\n");
	return -1;

   }

   if(argv[1] &&(!strcmp(argv[1],"read"))) consumatore(id_shm);


   if(fork())
   {
        wait(&STATUS);
   }
   else
   {
       produttore(id_shm);
   }

   if(fork())
   {
        wait(&STATUS);
   }
   else
   {
       consumatore(id_shm);
   }

//   ret = shmctl(id_shm, IPC_RMID, point);  

}/* end main */

