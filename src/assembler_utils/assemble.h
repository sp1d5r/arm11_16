#ifndef assemble
#define assemble

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define MAX_LINE_LENGTH 511
#define TRUECOND 0xe0000000

typedef struct SymbTable
{
  char **labels;
  int *memoryAddresses;
  int numberOfItems;
} SymbTable;

typedef enum MNEMONICS
{
  ADD,
  SUB,
  RSB,
  AND,
  EOR,
  ORR,
  MOV,
  TST,
  TEQ,
  CMP,
  MUL,
  MLA,
  LDR,
  STR,
  BEQ,
  BNE,
  BGE,
  BLT,
  BGT,
  BLE,
  B,
  ANDEQ,
  LSL,
  LABEL
} MNEMONICS;

/*
-getMnemonic
-convertBranchToBinary
-convertDPToBinary
-convertLSLToBinary
-convertMultiplyToBinary
-convertSDTToBinary
*/
MNEMONICS getMnemonic(char **instruction);
u_int32_t convertBranchToBinary(char **instructions, SymbTable table, int address);
u_int32_t convertDPToBinary(char **instruction);
u_int32_t convertLSLToBinary(char **instruction);
u_int32_t convertMultiplyToBinary(char **instructions);
u_int32_t convertSDTToBinary(char **instructions, int current_instruction, int *total_instructions, int *finalNumbers, int noOfLabels);
int operandTotal(char **array);
void createStringArray(char **array, int length, int maxSize);

#endif