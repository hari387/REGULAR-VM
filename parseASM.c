#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "ASMtypedefs.c"

uint8_t getOpcode(char* op){
	if(strcmp(op,"nop") == 0){
		return 0;
	} else if(strcmp(op,"add") == 0){
		return 1;
	} else if(strcmp(op,"sub") == 0){
		return 2;
	} else if(strcmp(op,"and") == 0){
		return 3;
	} else if(strcmp(op,"orr") == 0){
		return 4;
	} else if(strcmp(op,"xor") == 0){
		return 5;
	} else if(strcmp(op,"not") == 0){
		return 6;
	} else if(strcmp(op,"lsh") == 0){
		return 7;
	} else if(strcmp(op,"ash") == 0){
		return 8;
	} else if(strcmp(op,"tcu") == 0){
		return 9;
	} else if(strcmp(op,"tcs") == 0){
		return 10;
	} else if(strcmp(op,"set") == 0){
		return 11;
	} else if(strcmp(op,"mov") == 0){
		return 12;
	} else if(strcmp(op,"ldw") == 0){
		return 13;
	} else if(strcmp(op,"stw") == 0){
		return 14;
	} else if(strcmp(op,"ldb") == 0){
		return 15;
	} else if(strcmp(op,"stb") == 0){
		return 16;
	} else {
		return 17; 
		// for macros, 17 is not an actual opcode written to binaries
		// all non-base instructions are macros
	}
}

InstructionType getInstructionType(int opcode){
	if(opcode==0)return OP;
	else if(opcode>=1 && opcode<=5)return OP_RA_RB_RC;
	else if(opcode==6)return OP_RA_RB;
	else if(opcode>=7 && opcode <= 10)return OP_RA_RB_RC;
	else if(opcode==11)return OP_RA_IMM16;
	else if(opcode>=12 && opcode<=16)return OP_RA_RB;
	else return OP; //unneeded except for getting rid of warning
}

Label* retrieveLabel(char* name,Label** labelTable);

int evalToken(char** tokenptr,Label** labelTable){
	Label* lookup;
	if(strcmp((*tokenptr),"sp")==0)return asm_SP;
	else if(strcmp((*tokenptr),"at0")==0)return asm_AT0;
	else if(strcmp((*tokenptr),"at1")==0)return asm_AT1;
	else if(strcmp((*tokenptr),"pc")==0)return asm_PC;
	else if(NULL != (lookup = retrieveLabel(*tokenptr,labelTable))){
		return lookup->address;
	}
	else if((*tokenptr)[0] == 'r'){
		(*tokenptr)++;
		return atoi(*tokenptr);
	} else {
		return calculate(*tokenptr);
	}
}