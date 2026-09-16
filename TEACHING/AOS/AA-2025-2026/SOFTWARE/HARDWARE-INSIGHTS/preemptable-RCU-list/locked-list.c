#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list.h"


void locked_list_init(locked_list * l){

	l->head = NULL;
	pthread_spin_init(&(l->lock),PTHREAD_PROCESS_PRIVATE);
	asm volatile("mfence");

}

int locked_list_search(locked_list *l, long key){

	element *p;

	pthread_spin_lock(&(l->lock));

	p = l->head;
	
	//actual list traversal		
	while(p!=NULL){
		if ( p->key == key){
			break;
		}
		p = p->next;
	}

	pthread_spin_unlock(&(l->lock));
	

	if (p) return 1;

	return 0;

}

int locked_list_insert(locked_list *l, long key){

	element * p;
	
	p = malloc(sizeof(element));

	if(!p) return 0;

	p->key = key;
	p->next = NULL;

	AUDIT
	printf("insertion: waiting for lock\n");

	pthread_spin_lock(&(l->lock));

	//traverse and insert
	p->next = l->head;
	l->head = p;

	p = l->head;
		
	AUDIT
	while(p){
		printf("elem %d\n",p->key);
		p = p->next;
	}

	pthread_spin_unlock(&(l->lock));
	
	return 1;

}

int locked_list_remove(locked_list *l, long key){

	element * p, *removed = NULL;
	

	pthread_spin_lock(&(l->lock));

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
	
	pthread_spin_unlock(&l->lock);

	if(removed){
		free(removed);
		return 1;
	}
	
	return 0;

}


