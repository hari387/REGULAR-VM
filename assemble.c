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

void assemble(FILE* file,FILE* bin,Label** labelTable,Macro** macroTable);

// assembler-defined macros

void include(char* token,FILE* bin,Label** labelTable,Macro** macroTable);

int main(int argc, char* argv[]){

	if(argc != 3){
		errExit("Usage: ./assemble fileName binaryName");
	}

	char* fileName = argv[1];
	FILE* file = fopen(fileName, "r");

	char* binFileName = argv[2];
	FILE* bin = fopen(binFileName,"wb");

	Label* labelTable[labelTableSize];
	Macro* macroTable[macroTableSize];

	for(int i = 0; i < labelTableSize;i++){
		labelTable[i] = NULL;
	}

	for(int i = 0; i < macroTableSize;i++){
		macroTable[i] = NULL;
	}

	generateBaseMacros(macroTable);

	int lineNumber = 0;

	ParseLabelsMacros(file,labelTable,macroTable,&lineNumber);

	fseek(file,0,SEEK_SET);

	printf("labels and macros parsed\n");

	for(int i = 0;i<labelTableSize;i++){
		if(labelTable[i] != NULL){
			printf("|%s:%d|\n",labelTable[i]->name,labelTable[i]->address);
		}
	}

	assemble(file,bin,labelTable,macroTable);

	fclose(file);
	fclose(bin);

	return 0;
}

void assemble(FILE* file,FILE* bin,Label** labelTable,Macro** macroTable){

	char line[256];

	char* token;

	uint8_t opcode = 0;
	uint32_t empty = 0;
	uint8_t reg = 0;
	uint8_t rA = 0,rB = 0,rC = 0;
	uint8_t imm8 = 0;
	uint16_t imm16 = 0;

	Macro* macro;
	Instruction macroInstr;
	uint8_t constArg8;
	uint16_t constArg16;

	uint8_t macroDef = 0;

	while (fgets(line, sizeof(line), file)) {
	/* note that fgets don't strip the terminating \n, checking its
	presence would allow to handle lines longer that sizeof(line) */

		line[strlen(line)-2] = '\0'; //get rid of \n character at end of line

		token = strtok(line," ");

		// special cases
		if(strcmp(token,"inc") == 0){
			include(token,bin,labelTable,macroTable);
			continue;
		} else if(strcmp(token,"endProgram") == 0){
			uint32_t m = UINT32_MAX;
			fwrite(&m,4,1,bin);
			continue;
		} else if(strcmp(token,"def") == 0){
			macroDef = 1;
			continue;
		} else if(macroDef == 1){
			if(strcmp(token,"end") == 0){
				macroDef = 0;
			}
			continue;
		}

		opcode = getOpcode(token);
		InstructionType instrType = getInstructionType(opcode);

		if(opcode == 17 && strcmp(token,"label") != 0){ // if it is not a base instruction, it is a macro or label
			macroInstr.instruction32 = 0;
			macro = getMacro(macroTable,token);
			for(int i = 0; i < macro->argSize; i++){
				token = strtok(NULL," ");
				if(macro->instrType == OP_RA_IMM16 && i == 1){
					constArg16 = evalToken(&token,labelTable);
					macroInstr.opraimm.imm16 = constArg16;
				} else {
					constArg8 = evalToken(&token,labelTable);
					macroInstr.instruction32 += (constArg8 << (8*(i+1)));
				}
			}
			writeMacro(macro,macroInstr,bin);
			continue;
		}

		switch(instrType){

			case OP:
			write_OP(opcode,bin);
			break;

			case OP_RA:
			token = strtok(NULL," ");
			rA = evalToken(&token,labelTable);
			write_OP_RA(opcode,rA,bin);
			break;

			case OP_RA_IMM16:
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

			case OP_RA_RB_IMM8:
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
void include(char* token,FILE* bin,Label** labelTable,Macro** macroTable){
	FILE* header;
	token = strtok(NULL," ");
	header = fopen(token,"r");
	assemble(header,bin,labelTable,macroTable);
	fclose(header);
}