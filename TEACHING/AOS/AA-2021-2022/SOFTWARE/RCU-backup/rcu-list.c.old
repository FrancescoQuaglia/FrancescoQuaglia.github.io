#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"


void rcu_list_init(rcu_list * l){

	l->epoch = 0;
	l->standing[0] = 0;
	l->standing[1] = 0;
	l->head = NULL;
	pthread_spin_init(&l->write_lock,PTHREAD_PROCESS_PRIVATE);
	asm volatile("mfence");

}

int rcu_list_search(rcu_list *l, long key){
	int my_epoch = l->epoch;
	asm volatile("mfence");
	__sync_fetch_and_add(&l->standing[my_epoch],1);
	element *p = l->head;
	
	//actual list traversal		
	while(p!=NULL){
		if ( p->key == key){
			break;
		}
		p = p->next;
	}

	
	__sync_fetch_and_add(&l->standing[my_epoch],-1);

	if (p) return 1;

	return 0;

}

int rcu_list_insert(rcu_list *l, long key){

	element * p;
	int temp;
	int grace_epoch;;
	
	p = malloc(sizeof(element));

	if(!p) return 0;

	p->key = key;
	p->next = NULL;

	AUDIT
	printf("insertion: waiting for lock\n");

	pthread_spin_lock(&(l->write_lock));

	//traverse and insert
	p->next = l->head;
	l->head = p;

	//goto done;
	
	//move to a new epoch - still under write lock
	grace_epoch = temp = l->epoch;
	temp +=1;
	temp = temp%2;
	while(l->standing[temp] > 0);
	l->epoch = temp;
	asm volatile("mfence");

	AUDIT
	printf("insertion: waiting grace-full period on epoch %d\n",grace_epoch);
	
	while(l->standing[grace_epoch] > 0);
	

	p = l->head;
		
	AUDIT
	while(p){
		printf("elem %d\n",p->key);
		p = p->next;
	}

done:
	pthread_spin_unlock(&l->write_lock);
	
	return 1;

}

int rcu_list_remove(rcu_list *l, long key){

	element * p, *removed = NULL;
	int temp;
	int grace_epoch;;
	

	pthread_spin_lock(&l->write_lock);

	//traverse and delete
	p = l->head;

	if(p != NULL && p->key == key){
		removed = p;
		l->head = removed->next;
	}
	else{
		while(p != NULL){
			if ( p->next != NULL && p->next->key == key){
				removed = p->next;
				p->next = p->next->next;
				break;
			}	
			p = p->next;	
		}
	}
	//move to a new epoch - still under write lock
	grace_epoch = temp = l->epoch;
	temp +=1;
	temp = temp%2;
	while(l->standing[temp] > 0);
	l->epoch = temp;
	asm volatile("mfence");

	AUDIT
	printf("deletion: waiting grace-full period on epoch %d\n",grace_epoch);

	while(l->standing[grace_epoch] > 0);
	
	pthread_spin_unlock(&l->write_lock);

	if(removed){
		free(removed);
		return 1;
	}
	
	return 0;

}


