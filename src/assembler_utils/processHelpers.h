#ifndef helper
#define helper

#include "assemble.h"


void createStringArray(char **array, int length, int maxSize);
int operandTotal(char **array);
int getInt(char *values);
int returnAddressFromSymbolTable(char *stringTarget, SymbTable lookUp);
u_int32_t getShiftNum(char *shift);
u_int32_t getShiftAmount(char **shiftOperand);
u_int32_t processOffset(int offset);
u_int32_t processOperand2(char **operand2);


#endif