#pragma once

#include <stdlib.h>
#include <stdio.h>
#include "error.h"

typedef struct Token{
	int num;
	int isNum;
	// if(!isNum){0 is +, 1 is -, 2 is *, 3 is /, 4 is (, 5 is ) }
} Token;

typedef struct deqNode{
	struct deqNode* next;
	struct deqNode* prev;
	Token val;
} deqNode;

typedef struct deque{
	deqNode* head;
	deqNode* tail;
	int size;
} deque;

void printDeque(deque* d){
	deqNode* curr = d->head;
	while(curr != NULL){
		if(curr->val.isNum){
			printf("Num: %d\n",curr->val.num);
		} else {
			char c;
			switch(curr->val.num){
				case 0: c = '+';break;
				case 1: c = '-';break;
				case 2: c = '*';break;
				case 3: c = '/';break;
				case 4: c = '(';break;
				case 5: c = ')';break;
				default: c = 'F';break;
			}
			printf("Op: %c\n",c);
		}
		curr = curr->next;
	}
}

deque* initDeque(){
	deque* d = (deque*)malloc(sizeof(deque));
	d->head = NULL;
	d->tail = NULL;
	d->size = 0;
	return d;
}

void delDeque(deque* d){
	deqNode* curr = d->head;
	if(curr == NULL){
		free(d);
		return;
	}
	deqNode* second = d->head->next;
	while(second != NULL){
		free(curr);
		curr = second;
		second = second->next;
	}
	free(curr);
	free(d);
}

Token pop_back_deque(deque* d){
	if(d->head == NULL){
		errExit("Popped from empty deque");
	}
	Token result;
	result = d->tail->val;
	if(d->head == d->tail){
		free(d->tail);
		d->tail = NULL;
		d->head = NULL;
	} else {
		deqNode* last = d->tail->prev;
		free(d->tail);
		d->tail = last;
	}
	d->size--;
	return result;
}

Token pop_front_deque(deque* d){
	if(d->head == NULL){
		errExit("Popped from empty deque");
	}
	Token result;
	result = d->head->val;
	if(d->head == d->tail){
		free(d->head);
		d->head = NULL;
		d->tail = NULL;
	} else {
		deqNode* first = d->head->next;
		free(d->head);
		d->head = first;
	}
	d->size--;
	return result;
}

void push_back_deque(deque* d,Token t){
	if(d->head == NULL){
		d->head = (deqNode*)malloc(sizeof(deqNode));
		d->head->val = t;
		d->head->prev = NULL;
		d->head->next = NULL;
		d->tail = d->head;
	} else {
		deqNode* last = (deqNode*)malloc(sizeof(deqNode));
		last->val = t;
		last->prev = d->tail;
		last->next = NULL;
		d->tail->next = last;
		d->tail = last;
	}
	d->size++;
}

void push_front_deque(deque* d,Token t){
	if(d->head == NULL){
		d->head = (deqNode*)malloc(sizeof(deqNode));
		d->head->val = t;
		d->head->prev = NULL;
		d->head->next = NULL;
		d->tail = d->head;
	} else {
		deqNode* first = (deqNode*)malloc(sizeof(deqNode));
		first->val = t;
		first->next = d->head;
		first->prev = NULL;
		d->head->prev = first;
		d->head = first;
	}
	d->size++;
}