#include "assemble.h"
#include "processHelpers.h"

u_int32_t convertLSLToBinary(char **instruction)
{
  char **movInstruction = (char **)calloc(6, sizeof(char *));
  createStringArray(movInstruction, 6, 10);
  movInstruction[0] = "mov";
  movInstruction[1] = instruction[1];
  movInstruction[2] = instruction[1];
  movInstruction[3] = "lsl";
  movInstruction[4] = instruction[2];
  movInstruction[5] = "end";

  u_int32_t returnValue = convertDPToBinary(movInstruction);
  return returnValue;
}