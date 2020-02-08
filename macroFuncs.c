#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "ASMtypedefs.c"

#include "util/macroVectors.c"

// OP,OP_RA,OP_RA_IMM,OP_RA_RB,OP_RA_RB_IMM,OP_RA_RB_RC


int macroHash(char* name){
	int sum = 0;
	int i = 0;
	while(name[i] != '\0'){
		sum += name[i];
		i++;
	}
	return sum%macroTableSize;
}

Macro* getMacro(Macro** macroTable,char* name){
	uint8_t hashedVal = macroHash(name);
	Macro* macro = macroTable[hashedVal];
	while(macro != NULL && strcmp(macro->name,name) != 0){
		macro = macro->link;
	}
	return macro;
}

void putMacro(Macro** macroTable,Macro* macro){
	uint8_t hashedVal = macroHash(macro->name);
	Macro* inTable = macroTable[hashedVal];
	if(inTable == NULL){
		macroTable[hashedVal] = macro;
	} else {
		while(inTable->link != NULL){
			inTable = inTable->link;
		}
		inTable->link = macro;
	}
}

Macro* copyMacro(Macro* macro) {

	Macro* newMacro = (Macro*)malloc(sizeof(Macro));
	newMacro->name = macro->name; //name values are not modified, can use same pointer. Therefore do not deep copy name.
	newMacro->argSize = macro->argSize;
	if(macro->argSize > 0){
		newMacro->args = (uint8_t*)malloc(macro->argSize*sizeof(uint8_t));
		memcpy(newMacro->args, macro->args,macro->argSize*sizeof(uint8_t));
	} else {
		newMacro->args = NULL;
	}
	newMacro->instruction = macro->instruction;
	newMacro->instrType = macro->instrType;

	newMacro->submacros = macro->submacros;
	// do not need deep copy because we do not change args of submacros. Saves a bunch of memory.
	// if size == 0 then submacros is NULL, so it also handles that.

	return newMacro;
}

void subFuncArgs(Instruction* instr,Instruction funcArgs,uint8_t* args,uint8_t argSize);

void writeMacro(Macro* macro,Instruction funcArgs,FILE* bin){
	Instruction instr;
	instr.instruction32 = macro->instruction.instruction32;
	subFuncArgs(&instr,funcArgs,macro->args,macro->argSize);
	if(macro->submacros == NULL){
		fwrite(&instr,4,1,bin);
	} else {
		// instr is the new funcArgs because we swap args when calling macros in macros
		for(int i = 0;i<macro->submacros->size;i++){
			writeMacro(macro->submacros->arr[i],instr,bin);
		}
	}
}

void subFuncArgs(Instruction *instr,Instruction funcArgs,uint8_t* args,uint8_t argSize){
	for(int i = 1;i<=argSize;i++){
		if(args[i-1] != 0){
			switch(args[i-1]){
				case RA:
				instr->instruction32 += (funcArgs.opra.ra << 8*i);
				break;
				case RB:
				instr->instruction32 += (funcArgs.oprarb.rb << 8*i);
				break;
				case RC:
				instr->instruction32 += (funcArgs.oprarbrc.rc << 8*i);
				break;
				case IMM8:
				instr->instruction32 += (funcArgs.oprarbimm.imm8 << 8*i);
				break;
				case IMM16:
				instr->instruction32 += (funcArgs.opraimm.imm16 << 8*i);
				break;
			}
		}
	}
}

void generateBaseMacros(Macro** macroTable){

	Macro* nop = (Macro*)malloc(sizeof(Macro));
	nop->name = "nop";
	nop->args = NULL;
	nop->argSize = 0;
	nop->instruction.instruction32 = NOP;
	nop->instrType = OP;
	nop->submacros = NULL;
	nop->link = NULL;
	putMacro(macroTable,nop);

	Macro* add = (Macro*)malloc(sizeof(Macro));
	add->name = "add";
	add->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	add->argSize = 3;
	add->args[0] = 1; add->args[1] = 1; add->args[2] = 3;
	add->instruction.instruction32 = ADD;
	add->instrType = OP_RA_RB_RC;
	add->submacros = NULL;
	add->link = NULL;
	putMacro(macroTable,add);

	Macro* sub = (Macro*)malloc(sizeof(Macro));
	sub->name = "sub";
	sub->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	sub->argSize = 3;
	sub->args[0] = 1; sub->args[1] = 1; sub->args[2] = 3;
	sub->instruction.instruction32 = SUB;
	sub->instrType = OP_RA_RB_RC;
	sub->submacros = NULL;
	sub->link = NULL;
	putMacro(macroTable,sub);

	Macro* and = (Macro*)malloc(sizeof(Macro));
	and->name = "and";
	and->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	and->argSize = 3;
	and->args[0] = 1; and->args[1] = 1; and->args[2] = 3;
	and->instruction.instruction32 = AND;
	and->instrType = OP_RA_RB_RC;
	and->submacros = NULL;
	and->link = NULL;
	putMacro(macroTable,and);

	Macro* orr = (Macro*)malloc(sizeof(Macro));
	orr->name = "orr";
	orr->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	orr->argSize = 3;
	orr->args[0] = 1; orr->args[1] = 2; orr->args[2] = 3;
	orr->instruction.instruction32 = ORR;
	orr->instrType = OP_RA_RB_RC;
	orr->submacros = NULL;
	orr->link = NULL;
	putMacro(macroTable,orr);

	Macro* xor = (Macro*)malloc(sizeof(Macro));
	xor->name = "xor";
	xor->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	xor->argSize = 3;
	xor->args[0] = 1; xor->args[1] = 2; xor->args[2] = 3;
	xor->instruction.instruction32 = XOR;
	xor->instrType = OP_RA_RB_RC;
	xor->submacros = NULL;
	xor->link = NULL;
	putMacro(macroTable,xor);

	Macro* not = (Macro*)malloc(sizeof(Macro));
	not->name = "not";
	not->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	not->argSize = 2;
	not->args[0] = 1; not->args[1] = 2;
	not->instruction.instruction32 = NOT;
	not->instrType = OP_RA_RB;
	not->submacros = NULL;
	not->link = NULL;
	putMacro(macroTable,not);

	Macro* lsh = (Macro*)malloc(sizeof(Macro));
	lsh->name = "lsh";
	lsh->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	lsh->argSize = 3;
	lsh->args[0] = 1; lsh->args[1] = 2; lsh->args[2] = 3;
	lsh->instruction.instruction32 = LSH;
	lsh->instrType = OP_RA_RB_RC;
	lsh->submacros = NULL;
	lsh->link = NULL;
	putMacro(macroTable,lsh);

	Macro* ash = (Macro*)malloc(sizeof(Macro));
	ash->name = "ash";
	ash->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	ash->argSize = 3;
	ash->args[0] = 1; ash->args[1] = 2; ash->args[2] = 3;
	ash->instruction.instruction32 = ASH;
	ash->instrType = OP_RA_RB_RC;
	ash->submacros = NULL;
	ash->link = NULL;
	putMacro(macroTable,ash);

	Macro* tcu = (Macro*)malloc(sizeof(Macro));
	tcu->name = "tcu";
	tcu->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	tcu->argSize = 3;
	tcu->args[0] = 1; tcu->args[1] = 2; tcu->args[2] = 3;
	tcu->instruction.instruction32 = TCU;
	tcu->instrType = OP_RA_RB_RC;
	tcu->submacros = NULL;
	tcu->link = NULL;
	putMacro(macroTable,tcu);

	Macro* tcs = (Macro*)malloc(sizeof(Macro));
	tcs->name = "tcs";
	tcs->args = (uint8_t*)malloc(sizeof(uint8_t)*3);
	tcs->argSize = 3;
	tcs->args[0] = 1; tcs->args[1] = 2; tcs->args[2] = 3;
	tcs->instruction.instruction32 = TCS;
	tcs->instrType = OP_RA_RB_RC;
	tcs->submacros = NULL;
	tcs->link = NULL;
	putMacro(macroTable,tcs);

	Macro* set = (Macro*)malloc(sizeof(Macro));
	set->name = "set";
	set->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	set->argSize = 2;
	set->args[0] = 1; set->args[1] = 2;
	set->instruction.instruction32 = SET;
	set->instrType = OP_RA_IMM16;
	set->submacros = NULL;
	set->link = NULL;
	putMacro(macroTable,set);

	Macro* mov = (Macro*)malloc(sizeof(Macro));
	mov->name = "mov";
	mov->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	mov->argSize = 2;
	mov->args[0] = 1; mov->args[1] = 2;
	mov->instruction.instruction32 = MOV;
	mov->instrType = OP_RA_RB;
	mov->submacros = NULL;
	mov->link = NULL;
	putMacro(macroTable,mov);

	Macro* ldw = (Macro*)malloc(sizeof(Macro));
	ldw->name = "ldw";
	ldw->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	ldw->argSize = 2;
	ldw->args[0] = 1; ldw->args[1] = 2;
	ldw->instruction.instruction32 = LDW;
	ldw->instrType = OP_RA_RB;
	ldw->submacros = NULL;
	ldw->link = NULL;
	putMacro(macroTable,ldw);

	Macro* stw = (Macro*)malloc(sizeof(Macro));
	stw->name = "stw";
	stw->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	stw->argSize = 2;
	stw->args[0] = 1; stw->args[1] = 2;
	stw->instruction.instruction32 = STW;
	stw->instrType = OP_RA_RB;
	stw->submacros = NULL;
	stw->link = NULL;
	putMacro(macroTable,stw);

	Macro* ldb = (Macro*)malloc(sizeof(Macro));
	ldb->name = "ldb";
	ldb->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	ldb->argSize = 2;
	ldb->args[0] = 1; ldb->args[1] = 2;
	ldb->instruction.instruction32 = LDB;
	ldb->instrType = OP_RA_RB;
	ldb->submacros = NULL;
	ldb->link = NULL;
	putMacro(macroTable,ldb);

	Macro* stb = (Macro*)malloc(sizeof(Macro));
	stb->name = "stb";
	stb->args = (uint8_t*)malloc(sizeof(uint8_t)*2);
	stb->argSize = 2;
	stb->args[0] = 1; stb->args[1] = 2;
	stb->instruction.instruction32 = STB;
	stb->instrType = OP_RA_RB;
	stb->submacros = NULL;
	stb->link = NULL;
	putMacro(macroTable,stb);
}

