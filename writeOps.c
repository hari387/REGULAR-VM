#pragma once

#include <stdlib.h>
#include <stdint.h>

#include "error.h"

// OP,OP_RA,OP_RA_IMM,OP_RA_RB,OP_RA_RB_IMM,OP_RA_RB_RC

void write_OP(uint8_t opcode,FILE* bin){
	fputc(opcode,bin);
	fputc(0,bin);
	fputc(0,bin);
	fputc(0,bin);
}

void write_OP_RA(uint8_t opcode,uint8_t rA,FILE* bin){
	fputc(opcode,bin);
	fputc(rA,bin);
	fputc(0,bin);
	fputc(0,bin);
}

void write_OP_RA_IMM(uint8_t opcode,uint8_t rA,uint16_t imm16,FILE* bin){
	fputc(opcode,bin);
	fputc(rA,bin);
	fwrite(&imm16,2,1,bin);
}

void write_OP_RA_RB(uint8_t opcode,uint rA,uint8_t rB,FILE* bin){
	fputc(opcode,bin);
	fputc(rA,bin);
	fputc(rB,bin);
	fputc(0,bin);
}

void write_OP_RA_RB_IMM(uint8_t opcode,uint8_t rA,uint8_t rB,uint8_t imm8,FILE * bin){
	fputc(opcode,bin);
	fputc(rA,bin);
	fputc(rB,bin);
	fputc(imm8,bin);
}

void write_OP_RA_RB_RC(uint8_t opcode,uint8_t rA,uint8_t rB,uint8_t rC,FILE * bin){
	fputc(opcode,bin);
	fputc(rA,bin);
	fputc(rB,bin);
	fputc(rC,bin);
}