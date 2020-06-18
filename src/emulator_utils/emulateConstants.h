/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains all the constants required by the main emulator program.
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
#define FETCHED 0
#define DECODED 1
#define INIT_ZERO_VAL 0
#define NUMBER_OF_BYTES_IN_INSTR 4
#define TYPEMASK 0x0060

// enum for shift types:
typedef enum SHIFTTYPE
{
	LOGICALLEFT, LOGICALRIGHT, ARITHMETICRIGHT, ROTATERIGHT
} SHIFTTYPE;

// enum for the instructions available:
typedef enum INSTRUCTION
{
	MULTIPLY, BRANCH, PROCESS, TRANSFER, NONE
} INSTRUCTION;

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