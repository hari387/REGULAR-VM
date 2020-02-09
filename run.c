#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "error.h"

typedef enum ops{NOP,ADD,SUB,AND,ORR,XOR,NOT,LSH,ASH,TCU,TCS,SET,MOV,LDW,STW,LDB,STB} ops;

void executeInstruction(uint8_t opcode,uint32_t registers[],uint32_t* pc, uint32_t* sp, uint32_t memory[],int programBegin);

int main(int argc,char* argv[]){

	uint32_t registers[32];

	uint32_t memory[10000];
	for(int i =0;i<10000;i++){
		memory[i] = 0;
	}

	for(int i=0;i<32;i++)
		registers[i]=0;

	uint32_t* sp = registers+31;// stack pointer
	*sp = 1000; // start of stack
	uint32_t* at = registers+30;// assembler temporary
	uint32_t* pc = registers;// program counter

	FILE* bin = fopen(argv[1],"rb");

	int programEnd = 0;
	int programBegin = programEnd;
	while(fread(memory+programEnd,4,1,bin) == 1){
		programEnd++;
	}
	memory[programEnd] = UINT32_MAX;

	uint8_t opcode;

	while(1){
		opcode = memory[*pc+programBegin];
		if(opcode == UINT8_MAX)break;
		executeInstruction(opcode,registers,pc,sp,memory,programBegin);
		*pc+=1;
		//printf("Registers: ");
		//for(int i =0;i<32;i++)
		//	printf("%u,",registers[i]);
		//printf("\nStack: ");
		//for(int i =0;i<10;i++){
		//	printf("%u,",memory[1000+i]);
		//}
		//getchar();
	}
	printf("Registers: ");
	for(int i =0;i<32;i++){
		printf("%u,",registers[i]);
	}
	printf("\nStack: ");
	for(int i =0;i<10;i++){
		printf("%u,",memory[1000+i]);
	}
	printf("\n");

	return 0;
}

void executeInstruction(uint8_t opcode,uint32_t registers[],uint32_t* pc, uint32_t* sp,uint32_t memory[],int programBegin){
	uint8_t empty = 0;
	uint8_t rA = memory[*pc+programBegin] >> 8;
	uint8_t rB = memory[*pc+programBegin] >> 16;
	uint8_t rC = memory[*pc+programBegin] >> 24;
	//printf("Program Counter: %u\n",*pc);
	uint8_t imm8 = rC;
	uint16_t imm16 = ((uint16_t)rC << 8) | rB;
	uint32_t trash = 0;

	switch(opcode){
		case NOP:
			//printf("nop\n");
			break;
		case ADD:
			registers[rA] = (uint32_t)registers[rB] + (uint32_t)registers[rC];
			//printf("add %d %d %d\n",rA,rB,rC);
			break;
		case SUB:
			registers[rA] = (uint32_t)registers[rB] - (uint32_t)registers[rC];
			//printf("sub %d %d %d\n",rA,rB,rC);
			break;
		case AND:
			registers[rA] = registers[rB] & registers[rC];
			//printf("and %d %d %d\n",rA,rB,rC);
			break;
		case ORR:
			registers[rA] = registers[rB] | registers[rC];
			//printf("orr %d %d %d\n",rA,rB,rC);
			break;
		case XOR:
			registers[rA] = registers[rB] ^ registers[rC];
			//printf("xor %d %d %d\n",rA,rB,rC);
			break;
		case NOT:
			registers[rA] = ~registers[rB];
			//printf("not %d %d\n",rA,rB);
			break;
		case LSH:
			if(registers[rC] >= 32 && registers[rC] <= -32){ //signed - unsigned comparison is weird
				errExit("Shifting 32 bit int by more than 32");
			} else if((int32_t)registers[rC] < 0){
				registers[rA] = registers[rB] >> -(int32_t)registers[rC];
			} else {
				registers[rA] = registers[rB] << (int32_t)registers[rC];
			}
			//printf("lsh %d %d %d\n",rA,rB,rC);
			break;
		case ASH:
			if(registers[rC] >= 32 || registers[rC] <= -32){
				errExit("Shifting 32 bit int by more than 32");
			} else if(registers[rC] < 0){
				registers[rA] = (registers[rB] >> -registers[rC]) + (registers[rB]%(uint32_t)pow(2,31))/pow(2,31);
			} else {
				registers[rA] = registers[rB] << registers[rC];
			}
			//printf("ash %d %d %d\n",rA,rB,rC);
			break;
		case TCU:
			if(registers[rB] == registers[rC]){
				registers[rA] = 0;
			} else if((uint32_t)registers[rB] > (uint32_t)registers[rC]){
				registers[rA] = 1;
			} else {
				registers[rA] = -1;
			}
			//printf("tcu %d %d %d\n",rA,rB,rC);
			break;
		case TCS:
			if(registers[rB] == registers[rC]){
				registers[rA] = 0;
			} else if((int32_t)registers[rB] > (int32_t)registers[rC]){
				registers[rA] = 1;
			} else {
				registers[rA] = -1;
			}
			//printf("tcs %d %d %d\n",rA,rB,rC);
			break;
		case SET:
			registers[rA] = ((int16_t)imm16);
			//printf("set %d %d\n",rA,rB);
			break;
		case MOV:
			registers[rA] = registers[rB];
			//printf("mov %d %d\n",rA,rB);
			break;
		case LDW:
			registers[rA] = memory[registers[rB]];
			//printf("ldw %d %d\n",rA,rB);
			break;
		case STW:
			memory[registers[rA]] = registers[rB];
			//printf("stw %d %d\n",rA,rB);
			break;
		case LDB:
			registers[rA] = (uint8_t)(memory[registers[rB]]);
			//printf("ldb %d %d\n",rA,rB);
			break;
		case STB:
			memory[registers[rA]] = (uint8_t)(registers[rB]);
			//printf("stb %d %d\n",rA,rB);
			break;
	}
	return;
}