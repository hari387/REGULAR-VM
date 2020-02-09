#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "util/macroVectors.c"

#define asm_PC 0
#define asm_AT 30
#define asm_SP 31

#define macroTableSize 100

#define labelTableSize 100

typedef struct Label {
	char* name;
	int address;
	struct Label* next; // for hash table of labels
} Label;

typedef enum InstructionType {OP,OP_RA,OP_RA_IMM16,OP_RA_RB,OP_RA_RB_IMM8,OP_RA_RB_RC} InstructionType;

typedef enum ops {NOP,ADD,SUB,AND,ORR,XOR,NOT,LSH,ASH,TCU,TCS,SET,MOV,LDW,STW,LDB,STB} ops;

typedef enum argTypes {NONE,RA,RB,RC,IMM8,IMM16} argTypes;

typedef struct instr_OP {
	uint8_t opcode;
} instr_OP;

typedef struct instr_OP_RA {
	uint8_t opcode;
	uint8_t ra;
} instr_OP_RA;

typedef struct instr_OP_RA_IMM {
	uint8_t opcode;
	uint8_t ra;
	uint16_t imm16;
} instr_OP_RA_IMM;

typedef struct instr_OP_RA_RB {
	uint8_t opcode;
	uint8_t ra;
	uint8_t rb;
} instr_OP_RA_RB;

typedef struct instr_OP_RA_RB_IMM {
	uint8_t opcode;
	uint8_t ra;
	uint8_t rb;
	uint8_t imm8;
} instr_OP_RA_RB_IMM;

typedef struct instr_OP_RA_RB_RC {
	uint8_t opcode;
	uint8_t ra;
	uint8_t rb;
	uint8_t rc;
} instr_OP_RA_RB_RC;

typedef union Instruction {
	uint32_t instruction32;
	instr_OP op;
	instr_OP_RA opra;
	instr_OP_RA_IMM opraimm;
	instr_OP_RA_RB oprarb;
	instr_OP_RA_RB_IMM oprarbimm;
	instr_OP_RA_RB_RC oprarbrc;
} Instruction;

typedef struct Macro {
	char *name;
	InstructionType instrType;
	uint8_t* args;
	uint8_t argSize;
	Instruction instruction;
	vector* submacros; // list of pointers to submacros
	struct Macro* link; // for hash table of macros
} Macro;