#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>


#define  AUDIT if(0) //this is a general audit flag

typedef struct _element{
	struct _element * next;
	long key;
} element;


#ifndef LOCK //the below stuff related to the RCU version of code

//this defines the RCU house keeping period
#ifndef PERIOD
#define PERIOD 1
#endif


#define EPOCHS (2) //we have the current and the past epoch only

typedef struct _rcu_list{
	unsigned long standing[EPOCHS];	//you can further optimize putting these values
					//on different cache lines
	unsigned long epoch; //a different cache line for this can also help
	int next_epoch_index;
	pthread_spinlock_t write_lock;
	element * head;
} __attribute__((packed)) rcu_list;

typedef rcu_list list __attribute__((aligned(64)));

#define MASK 0x8000000000000000


#define list_insert rcu_list_insert
#define list_search rcu_list_search
#define list_remove rcu_list_remove
#define list_init rcu_list_init

//RCU versions
void rcu_list_init(rcu_list * l);

int rcu_list_search(rcu_list *l, long key);

int rcu_list_insert(rcu_list *l, long key);

int rcu_list_remove(rcu_list *l, long key);


#else //the blow stuff related to the LOCK based version of code

typedef struct _locked_list{
	pthread_spinlock_t lock;
	element * head;
} locked_list;

typedef locked_list list;

#define list_insert locked_list_insert
#define list_search locked_list_search
#define list_remove locked_list_remove
#define list_init locked_list_init

//lock-based versions

void locked_list_init(locked_list * l);

int locked_list_search (locked_list *l, long key);

int locked_list_insert(locked_list *l, long key);

int locked_list_remove(locked_list *l, long key);
#endif
