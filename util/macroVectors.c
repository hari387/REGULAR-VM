#pragma once

#include <stdlib.h>
#include <string.h>

#include "../error.h"

struct Macro;

#define vectorType struct Macro*

typedef struct vector{
	int totalSize;
	int size;
	vectorType* arr;
} vector;

vector* initVec(){
	vector* vec = (vector*)malloc(sizeof(vector));
	vec->totalSize = 8;
	vec->size = 0;
	vec->arr = (vectorType*)malloc(8*sizeof(vectorType));
	return vec;
}

void delVec(vector* vec){
	free(vec->arr);
	free(vec);
}

void appendToVec(vector* vec, vectorType x){
	if(vec->size == vec->totalSize){ // allocate 2*current_memory and copy, then append
		vectorType* newarr = (vectorType*)malloc(vec->totalSize * 2 * sizeof(vectorType));
		memcpy(newarr,vec->arr,vec->totalSize * sizeof(vectorType));
		free(vec->arr);
		vec->arr = newarr;
		vec->totalSize = vec->totalSize * 2;
	}
	vec->arr[vec->size] = x;
	vec->size++;
}

vectorType popFromVec(vector* vec){
	if(vec->size == 0){
		errExit("Popped from empty vector");
	}
	vectorType result = vec->arr[vec->size - 1];
	vec->arr[vec->size - 1] = NULL;
	vec->size--;
	return result;
}