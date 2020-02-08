#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "ASMtypedefs.c"
#include "error.h"
#include "calculator/calculator.c"
#include "writeOps.c"
#include "labelFuncs.c"
#include "parseASM.c"

#define numberOfBaseInstructions 17

void assemble(FILE* file,FILE* bin,Label** labelTable);

// assembler-defined macros

void include(char* token,FILE* bin,Label** labelTable);

void pushc(char* token,FILE* bin,Label** labelTable);

void pushr(char* token,FILE* bin,Label** labelTable);

void pushret(char* token,FILE* bin,Label** labelTable);

void pop(char* token,FILE* bin,Label** labelTable);

int main(int argc, char* argv[]){

	if(argc != 3){
		error("Usage: ./assemble fileName binaryName");
	}

	char* fileName = argv[1];
	FILE* file = fopen(fileName, "r");

	char* binFileName = argv[2];
	FILE* bin = fopen(binFileName,"wb");

	Label* labelTable[labelTableSize];

	for(int i = 0; i < labelTableSize;i++){
		labelTable[i] = NULL;
	}

	int lineNumber = 0;

	labelParse(file,labelTable,&lineNumber);

	fseek(file,0,SEEK_SET);

	printf("label parsed\n");

	for(int i = 0;i<labelTableSize;i++){
		if(labelTable[i] != NULL){
			printf("|%s:%d|\n",labelTable[i]->name,labelTable[i]->address);
		}
	}

	assemble(file,bin,labelTable);

	fclose(file);
	fclose(bin);

	return 0;
}

void assemble(FILE* file,FILE* bin,Label** labelTable){

	char line[256];

	char* token;

	uint8_t opcode = 0;
	uint32_t empty = 0;
	uint8_t reg = 0;
	uint8_t rA = 0,rB = 0,rC = 0;
	uint8_t imm8 = 0;
	uint16_t imm16 = 0;

	uint8_t funcDef = 0;

	while (fgets(line, sizeof(line), file)) {
	/* note that fgets don't strip the terminating \n, checking its
	presence would allow to handle lines longer that sizeof(line) */

		line[strlen(line)-2] = '\0'; //get rid of \n character at end of line

		token = strtok(line," ");

		// include file
		if(strcmp(token,"inc") == 0){
			include(token,bin,labelTable);
			continue;
		} else if(strcmp(token,"pushc") == 0){
			pushc(token,bin,labelTable);
			continue;
		} else if(strcmp(token,"pushr") == 0){
			pushr(token,bin,labelTable);
			continue;
		} else if(strcmp(token,"pushret") == 0){
			pushret(token,bin,labelTable);
			continue;
		} else if(strcmp(token,"pop") == 0){
			pop(token,bin,labelTable);
			continue;
		} else if(strcmp(token,"endProgram") == 0){
			uint32_t m = UINT32_MAX;
			fwrite(&m,4,1,bin);
			continue;
		}

		opcode = getOpcode(token);
		instructionType instrType = getInstructionType(opcode);

		switch(instrType){

			case OP:
			write_OP(opcode,bin);
			break;

			case OP_RA:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			write_OP_RA(opcode,rA,bin);
			break;

			case OP_RA_IMM:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			imm16 = (uint16_t)evalToken(&token,labelTable);
			write_OP_RA_IMM(opcode,rA,imm16,bin);
			break;

			case OP_RA_RB:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			rB = evalToken(&token,labelTable);
			write_OP_RA_RB(opcode,rA,rB,bin);

			break;

			case OP_RA_RB_IMM:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			rB = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			imm8 = (uint8_t)evalToken(&token,labelTable);
			write_OP_RA_RB_IMM(opcode,rA,rB,imm8,bin);
			break;

			case OP_RA_RB_RC:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			rB = evalToken(&token,labelTable);
			token = strtok(NULL," ");
			rC = evalToken(&token,labelTable);
			write_OP_RA_RB_RC(opcode,rA,rB,rC,bin);
			break;

			default:
			break;
		}
	}
	/* may check feof here to make a difference between eof and io failure -- network
	timeout for instance */
	return;
}

// assembler-defined macros

// include a file
void include(char* token,FILE* bin,Label** labelTable){
	FILE* header;
	token = strtok(NULL," ");
	header = fopen(token,"r");
	assemble(header,bin,labelTable);
	fclose(header);
}

// push a constant onto the stack
void pushc(char* token,FILE* bin,Label** labelTable){
	uint8_t opcode;
	uint16_t imm16;

	token = strtok(NULL," ");

	opcode = getOpcode("set");
	imm16 = (uint16_t)evalToken(&token,labelTable);
	write_OP_RA_IMM(opcode,asm_AT,imm16,bin);

	opcode = getOpcode("stw");
	write_OP_RA_RB(opcode,asm_SP,asm_AT,bin);

	opcode = getOpcode("set");
	imm16 = 1;
	write_OP_RA_IMM(opcode,asm_AT,imm16,bin);

	opcode = getOpcode("add");
	write_OP_RA_RB_RC(opcode,asm_SP,asm_SP,asm_AT,bin);
}

// push the value of a register onto the stack
void pushr(char* token,FILE* bin,Label** labelTable){
	uint8_t opcode;
	uint8_t rA,rB,rC;
	uint16_t imm16;

	token = strtok(NULL," ");

	opcode = getOpcode("stw");
	rB = evalToken(&token,labelTable);
	write_OP_RA_RB(opcode,asm_SP,rB,bin);

	opcode = getOpcode("set");
	imm16 = 1;
	write_OP_RA_IMM(opcode,asm_AT,imm16,bin);

	opcode = getOpcode("add");
	write_OP_RA_RB_RC(opcode,asm_SP,asm_SP,asm_AT,bin);
}

// push the return address onto the stack. Function must be called immediately after.
void pushret(char* token,FILE* bin,Label** labelTable){
	uint8_t opcode;
	uint16_t imm16;

	opcode = getOpcode("set");
	imm16 = 4;
	write_OP_RA_IMM(opcode,asm_AT,imm16,bin);

	opcode = getOpcode("add");
	write_OP_RA_RB_RC(opcode,asm_AT,asm_AT,asm_PC,bin);

	char pushAT[] = "pushr at";
	token = strtok(pushAT," ");

	pushr(token,bin,labelTable);
}

// pop a uint32_t off the stack into the register in argument
void pop(char* token,FILE* bin,Label** labelTable){
	uint8_t opcode;
	uint8_t rA,rB,rC;
	uint16_t imm16;

	token = strtok(NULL," ");

	opcode = getOpcode("set");
	imm16 = 1;
	write_OP_RA_IMM(opcode,asm_AT,imm16,bin);

	opcode = getOpcode("sub");
	write_OP_RA_RB_RC(opcode,asm_SP,asm_SP,asm_AT,bin);

	opcode = getOpcode("ldw");
	rA = evalToken(&token,labelTable);
	write_OP_RA_RB(opcode,rA,asm_SP,bin);
}