/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains all the utilities required by the emulator.
*/

#ifndef ARM11_16_SRC_DEFINES_H_
#define ARM11_16_SRC_DEFINES_H_

// Required Libraries:
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Constant Definitions:
#define ARG_WITH_FILE 1
#define PROGRAM_COUNTER_LOCATION 15
#define NUMBER_OF_REGISTERS 17
#define MAX_BYTES 65536
#define CPSR_LOCATION 16
#define ALLOCATION_ERROR -1
#define NUMBER_OF_ARGUMENTS 2
#define BRANCH_INSTR_MASK 0x08000000
#define MULTIPLY_INSTR_MASK_1 0x000000F0
#define MULTIPLY_INSTR_MASK_2 0x0FC00000
#define TRANSFER_INSTR_MASK 0x04000000
#define DATA_RN_MASK 0x000F0000
#define DATA_RD_MASK 0x0000F000
#define IMMEDIATE_MASK 0x02000000
#define S_OR_L_MASK 0x00100000
#define U_MASK 0x00800000
#define P_MASK 0x01000000
#define MULTIPLY_RN_MASK 0x0000F000
#define MULTIPLY_RS_MASK 0x00000F00
#define MULTIPLY_RD_MASK 0x000F0000
#define MULTIPLY_RM_MASK 0x0000000F
#define ACCUMULATE_MASK 0x00200000
#define BITS_IN_NIBBLE 4
#define BITS_IN_BYTE 8
#define BITS_IN_TWO_BYTES 16
#define BITS_IN_THREE_BYTES 24
#define NUMBER_OF_BYTES_IN_INSTR 4
#define INIT_ZERO_VAL 0
#define OFFSET_MASK 0x00FFFFFF
#define OFFSET_SHIFT 2
#define SIGN_BIT_MASK 0x02000000
#define SIGN_EXTEND_MASK 0xFC000000
#define Z_AND_N_FLAG_MASK 0x3FFFFFFF
#define TYPEMASK 0x0060
#define CODE_MASK 0xF0000000
#define Z_FLAG_MASK 0x40000000
#define N_FLAG_MASK 0x80000000
#define V_FLAG_MASK 0x10000000
#define FETCHED 0
#define DECODED 1
#define OPCODE_MASK 0x0000000F
#define OFFSET_MASK_2 0x00000FFF

// enum for the instructions available:
typedef enum INSTRUCTION
{
	MULTIPLY, BRANCH, PROCESS, TRANSFER, NONE
} INSTRUCTION;

// enum for possible opcodes in data process instructions:
typedef enum OPCODE
{
	AND = 0, EOR = 1, SUB = 2, RSB = 3, ADD = 4, TST = 8, TEQ = 9, CMP = 10, ORR = 12, MOV = 13
} OPCODE;

// enum for shift types:
typedef enum SHIFTTYPE
{
	LOGICALLEFT, LOGICALRIGHT, ARITHMETICRIGHT, ROTATERIGHT
} SHIFTTYPE;

// Struct storing the current state of the machine:
typedef struct ARMSTATE
{
	uint8_t *memory;
	uint32_t regs[NUMBER_OF_REGISTERS];
	bool fetchDecodeExecute[2]; // Assists the pipeline.
} ARMSTATE;

// Struct to store the value of the 4 bit instruction in big endian, the instruction type, the
// registers which are required and the values of any other bits such as the Immediate operand:
typedef struct DECODE
{
	uint32_t bigEndianInstr;
	INSTRUCTION instruction;
	uint8_t destReg: 4, rn: 4;
	bool op1, op2;
} DECODE;


// Function Declarations:
void initialiseState(ARMSTATE *);
INSTRUCTION workOutType(uint32_t);
uint32_t bigEndianConverter(uint32_t, uint32_t, uint32_t, uint32_t);
void decodeHelper(uint32_t, INSTRUCTION, DECODE *);
void decode(const uint32_t *, DECODE *);
bool conditionChecker(uint32_t, ARMSTATE *);
void branch(DECODE *, ARMSTATE *);
void multiply(DECODE *, ARMSTATE *);
uint32_t rotateRight(uint32_t, uint8_t);
SHIFTTYPE shiftType(uint16_t);
bool shift(uint32_t *, SHIFTTYPE, uint8_t);
uint32_t shiftRegister(ARMSTATE *, uint8_t, uint16_t, bool *);
uint8_t getOpCode(uint32_t);
uint32_t processHelper(DECODE *, ARMSTATE *, bool *);
void process(DECODE *, ARMSTATE *);
uint32_t addOrSub(uint32_t, DECODE *, ARMSTATE *);
bool outOfBounds(uint32_t);
void load(ARMSTATE *, DECODE *, uint32_t);
void store(ARMSTATE *, DECODE *, uint32_t);
void transferHelper(uint32_t, DECODE *, ARMSTATE *);
void transfer(DECODE *, ARMSTATE *);
void execute(DECODE *, ARMSTATE *);
uint32_t getMem(ARMSTATE *, int);
void outputNonZeroMem(ARMSTATE *);
void outputState(ARMSTATE *);
void fetch(ARMSTATE *, uint32_t *);
void pipeline(ARMSTATE *);
bool isFileNull(FILE *);
int readFromBinFile(ARMSTATE *, FILE *);

#endif //ARM11_16_SRC_DEFINES_H_
