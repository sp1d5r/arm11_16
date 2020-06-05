#ifndef assemble
#define assemble

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 511
#define TRUECOND 0xe0000000

typedef struct SymbTable{
  char **labels;
  int *memoryAddresses;
  int numberOfItems;
} SymbTable;

typedef enum MNEMONICS{
  ADD,SUB,RSB,AND,EOR,ORR,MOV,TST,TEQ,CMP,MUL,MLA,LDR,STR,BEQ,BNE,BGE,BLT,BGT,BLE,B,ANDEQ,LABEL
}  MNEMONICS;

#endif
