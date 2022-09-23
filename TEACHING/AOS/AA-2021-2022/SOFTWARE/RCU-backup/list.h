#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <pthread.h>

#define ERAS (8)

#define  AUDIT if(0)

typedef struct _element{
	struct _element * next;
	long key;
} element;


#ifndef LOCK

#ifndef PERIOD
#define PERIOD 1
#endif

typedef struct _rcu_list{
	long standing[ERAS];
	int epoch;
	pthread_spinlock_t write_lock;
	element * head;
} rcu_list;

typedef rcu_list list;

#define list_insert rcu_list_insert
#define list_search rcu_list_search
#define list_remove rcu_list_remove
#define list_init rcu_list_init

//RCU versions
void rcu_list_init(rcu_list * l);

int rcu_list_search(rcu_list *l, long key);

int rcu_list_insert(rcu_list *l, long key);

int rcu_list_remove(rcu_list *l, long key);


#else

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
