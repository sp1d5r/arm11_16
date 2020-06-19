/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing constants and function definitions for the assembler.
*/

#ifndef assemble
#define assemble

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TRUE_CONDITION 0xe0000000
#define NUMBER_OF_COMMANDS 23
#define MAX_LINE_LENGTH 511
#define EXTRA_CHARS 5

typedef struct symbolTable
{
  char **labels;
  int numberOfItems;
  int *memoryAddresses;
} symbolTable;

typedef struct sdtHelper
{
  int no_instructions;
  int *finalNumbers;
  int sizeOfFinalNumbers;
} sdtHelper;

typedef enum Types
{
  DP = 0,
  SDT = 1,
  BRANCH = 2,
  MULTIPLY = 3,
  SPECIAL = 4
} Types;

typedef enum Dp
{
  ADD = 4,
  SUB = 2,
  RSB = 3,
  AND = 0,
  EOR = 1,
  ORR = 12,
  MOV = 13,
  TST = 8,
  TEQ = 9,
  CMP = 10
} Dp;

typedef enum Multiply
{
  MUL,
  MLA,
} Multiply;

typedef enum Dt
{
  LDR,
  STR
} Dt;

typedef enum Branch
{
  BEQ = 0,
  BNE = 1,
  BGE = 10,
  BLT = 11,
  BGT = 12,
  BLE = 13,
  B = 14
} Branch;

typedef enum Special
{
  LSL,
  HALT
} Special;

typedef struct mnemonicMap
{
  int mnemonic;
  char *str;
  Types t;
} mnemonicMap;

typedef struct instruction
{
  symbolTable symbolTable;
  union {
    Dp opCode;
    Dt sdt;
    Branch condCode;
  } u;
  char **lines;
  int lineCount;
  sdtHelper sdt_helper;
} instruction;

#define CHECK_IF_NULL(x)                        \
  if (!x)                                       \
  {                                             \
    printf("Allocation Error / Invalid File."); \
    exit(EXIT_FAILURE);                         \
  }

void assembler(char **);
void secondPass(instruction *, char *);
void process(int, mnemonicMap[], instruction *, FILE *);
void writeToFile(FILE *, uint32_t);
uint32_t littleEndianConverter(uint32_t);
void setInstrData(mnemonicMap, instruction *);
uint32_t convertBranchToBinary(instruction *, const int);
uint32_t convertSpecialToBinary(instruction *, const int);
uint32_t convertMultiplyToBinary(instruction *, const int);
uint32_t convertSdtToBinary(instruction *, const int);
int calculatePFlag(char *);
void updateInts(int *, int, int);
uint32_t convertDpToBinary(instruction *, const int);
char **splitUp(char *);
char **firstPass(char *, symbolTable *, int *);
void addEndToInstruction(char **, int);
void initialiseState(instruction *, symbolTable, int, char **);
int operandTotal(char **);
int getInt(char *);
int returnAddressFromSymbolTable(char *, symbolTable);
int getShiftNum(char *);
uint32_t getShiftAmount(char **);
uint32_t processOffset(int);
uint32_t processOperand2(char **);

#endif // assemble