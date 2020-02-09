#pragma once

#include <stdlib.h>
#include <stdio.h>

void errExit(char* errorCode){
	printf("Error: %s\n",errorCode);
	exit(1);
}