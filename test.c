#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "error.h"
#include "macroFuncs.c"

int main(int argc, char* argv[]){

	printf("hi1\n");
	
	Macro* macroTable[macroTableSize];
	for(int i = 0; i < macroTableSize;i++)
		macroTable[i] = NULL;
	generateBaseMacros(macroTable);
	
	Macro* add5 = (Macro*)malloc(sizeof(Macro));
	add5->name = "add5";
	add5->args = (uint8_t*)malloc(sizeof(uint8_t));
	add5->args[0] = 0;
	add5->argSize = 1;
	add5->instruction.instruction32 = 0;
	add5->instrType = OP_RA;
	add5->submacros = initVec();
	add5->link = NULL;
	
	appendToVec(add5->submacros,copyMacro(getMacro(macroTable,"set")));
	appendToVec(add5->submacros,copyMacro(getMacro(macroTable,"add")));

	add5->args[0] = RA;
	add5->submacros->arr[0]->instruction.instruction32 += (asm_AT << 8);
	add5->submacros->arr[0]->instruction.instruction32 += (5 << 16);
	add5->submacros->arr[0]->args[0] = NONE;
	add5->submacros->arr[0]->args[1] = NONE;
	add5->submacros->arr[1]->instruction.instruction32 += (asm_AT << 24);
	add5->submacros->arr[1]->args[0] = RA;
	add5->submacros->arr[1]->args[1] = RA;
	add5->submacros->arr[1]->args[2] = NONE;


	FILE* bin;
	bin = fopen("hello","wb");

	int input = atoi(argv[1]);

	Instruction instr;
	instr.instruction32 = 0;
	instr.opra.ra = input;
	fputc(0x0b,bin);
	fputc(input,bin);
	fputc(atoi(argv[2]),bin);
	fputc(0x0,bin);
	writeMacro(add5,instr,bin);

	fclose(bin);


	printf("hi2\n");

	return 0;
}