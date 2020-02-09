#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "error.h"

int main(int argc, char* argv[]){

	printf("hi1\n");
	
	char line[256];

	FILE* file = fopen("hi.txt", "r");

	while(fgets(line, sizeof(line),file)){
		printf("|%d|\n",line[1] == '\n');
		printf("[%c]",line[0]);
		printf("%s",line);
	}

	printf("hi2\n");

	return 0;
}