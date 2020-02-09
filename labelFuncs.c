#pragma once

#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "error.h"

#include "ASMtypedefs.c"

#include "parseASM.c"

#include "macroFuncs.c"

Label* initLabel(char* name,int address){
	Label* label = (Label*)malloc(sizeof(Label));
	label->name = (char*)malloc(strlen(name)+1);
	strcpy(label->name,name);
	label->address = address;
	label->next = NULL;
	return label;
}

int labelHash(char* name){
	int sum = 0;
	int i = 0;
	while(name[i] != '\0'){
		sum += name[i];
		i++;
	}
	return sum%labelTableSize;
}

void insertLabel(Label* label,Label** labelTable){
	int hashedVal = labelHash(label->name);
	Label* iter;
	if(labelTable[hashedVal] == NULL){
		labelTable[hashedVal] = label;
	} else {
		iter = labelTable[hashedVal];
		while(iter->next != NULL){
			iter = iter->next;
		}
		iter->next = label;
	}
	return;
}

Label* retrieveLabel(char* name,Label** labelTable){
	Label* mylabel = labelTable[labelHash(name)];
	while(mylabel != NULL && strcmp(name,mylabel->name) != 0){
		mylabel = mylabel->next;
	}
	return mylabel;
}

void ParseLabelsMacros(FILE* file,Label** labelTable,Macro** macroTable,int* lineNumber){
	char line[256];
	char* token;
	FILE* header;
	uint8_t macroDef = 0;
	Macro* macro;
	Macro* submacro;
	uint8_t constArg8;
	uint16_t constArg16;
	while(fgets(line,sizeof(line),file)){

		if(line[1] == '\n' || line[0] == '#'){
			continue;
		}

		line[strlen(line)-2] = '\0';
		token = strtok(line," ");

		if(macroDef){
			if(strcmp(token,"end") == 0){
				macroDef = 0;
				putMacro(macroTable,macro);
				macro = NULL;
				continue;
			}
			submacro = copyMacro(getMacro(macroTable,token));
			for(int i = 0; i <submacro->argSize;i++){
				token = strtok(NULL," ");
				if(strcmp(token,"rA") == 0){
					submacro->args[i] = RA;
				} else if(strcmp(token,"rB") == 0){
					submacro->args[i] = RB;
				} else if(strcmp(token,"imm16") == 0){
					submacro->args[i] = IMM16;
				} else if(strcmp(token,"rC") == 0){
					submacro->args[i] = RC;
				} else if(strcmp(token,"imm8") == 0){
					submacro->args[i] = IMM8;
				} else {
					submacro->args[i] = NONE;
					if(submacro->instrType == OP_RA_IMM16 && i == 1){
						constArg16 = evalToken(&token,labelTable);
						submacro->instruction.opraimm.imm16 = constArg16;
					} else {
						constArg8 = evalToken(&token,labelTable);
						submacro->instruction.instruction32 += (constArg8 << (8*(i+1)));
					}
				}
			}
			appendToVec(macro->submacros,submacro);
			submacro = NULL;
			continue;
		}

		if(strcmp(token,"label") == 0){
			token = strtok(NULL," ");
			Label* label = initLabel(token,*lineNumber);
			insertLabel(label,labelTable);
		} else if(strcmp(token,"inc") == 0){
			token = strtok(NULL," ");
			header = fopen(token,"r");
			ParseLabelsMacros(header,labelTable,macroTable,lineNumber);
			fclose(header);
		} else if(strcmp(token,"endProgram") == 0){
			(*lineNumber)++;
		} else if(strcmp(token,"def") == 0){
			macro = (Macro*)malloc(sizeof(Macro));
			token = strtok(NULL," ");
			macro->name = (char*)malloc(strlen(token)+1);
			strcpy(macro->name,token);
			token = strtok(NULL," ");
			if(strcmp(token,"OP") == 0){
				macro->instrType = OP;
				macro->argSize = 0;
				macro->args = NULL;
			} else if(strcmp(token,"OP_RA") == 0){
				macro->instrType = OP_RA;
				macro->argSize = 1;
				macro->args = (uint8_t *)malloc(macro->argSize*sizeof(uint8_t));
				macro->args[0] = RA;
			} else if(strcmp(token,"OP_RA_IMM16") == 0){
				macro->instrType = OP_RA_IMM16;
				macro->argSize = 2;
				macro->args = (uint8_t *)malloc(macro->argSize*sizeof(uint8_t));
				macro->args[0] = RA; macro->args[1] = IMM16;
			} else if(strcmp(token,"OP_RA_RB") == 0){
				macro->instrType = OP_RA_RB;
				macro->argSize = 2;
				macro->args = (uint8_t *)malloc(macro->argSize*sizeof(uint8_t));
				macro->args[0] = RA; macro->args[1] = RB;
			} else if(strcmp(token,"OP_RA_RB_IMM8") == 0){
				macro->instrType = OP_RA_RB_IMM8;
				macro->argSize = 3;
				macro->args = (uint8_t *)malloc(macro->argSize*sizeof(uint8_t));
				macro->args[0] = RA; macro->args[1] = RB; macro->args[2] = IMM8;
			} else if(strcmp(token,"OP_RA_RB_RC") == 0){
				macro->instrType = OP_RA_RB_RC;
				macro->argSize = 3;
				macro->args = (uint8_t *)malloc(macro->argSize*sizeof(uint8_t));
				macro->args[0] = RA; macro->args[1] = RB; macro->args[2] = RC;
			} else {
				errExit("Invalid Instruction Type: Valid instruction types are OP,OP_RA,OP_RA_IMM16,OP_RA_RB,OP_RA_RB_IMM8,OP_RA_RB_RC");
			}
			macro->instruction.instruction32 = 0;
			macro->submacros = initVec();
			macro->link = NULL;
			macroDef = 1;
		} else {
			macro = getMacro(macroTable,token);
			(*lineNumber) += macroSize(macro);
		}
	}
}