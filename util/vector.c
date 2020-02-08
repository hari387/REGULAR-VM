#pragma once

#include <stdlib.h>
#include <string.h>
#include "error.h"

typedef struct vector{
	int size;
	int used;
	int* begin;
} vector;

vector* initVec(){
	vector* vec = (vector*)malloc(sizeof(vector));
	vec->size = 8;
	vec->used = 0;
	vec->begin = (int*)malloc(8*sizeof(int));
	return vec;
}

void delVec(vector* vec){
	free(vec->begin);
	free(vec);
}

void appendToVec(vector* vec, int x){
	if(vec->used == vec->size){ // allocate 2*current_memory and copy, then append
		printf("double vector size\n");
		int* newarr = (int*)malloc(vec->size * 2 * sizeof(int));
		memcpy(newarr,vec->begin,vec->size * sizeof(int));
		free(vec->begin);
		vec->begin = newarr;
		vec->size = vec->size * 2;
	}
	vec->begin[vec->used] = x;
	vec->used++;
}

int popFromVec(vector* vec){
	if(vec->used == 0){
		error("Popped from empty vector");
	}
	int result = vec->begin[vec->used - 1];
	vec->begin[vec->used - 1] = 0;
	vec->used--;
	return result;
}

void printVec(vector* vec){
	for(int i = 0;i<vec->used;i++){
		printf("%d,",vec->begin[i]);
	}
}