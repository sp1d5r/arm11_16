#include "assemble.h"
#include "processHelpers.h"

/*
* convertMultiplyToBinary
* PRE - the instruction is a multiply instruction
* Params - instruction / array of strings
* Returns - the 32 bit integer equivalent of the multiply instruction
*/
u_int32_t convertMultiplyToBinary(char **instructions)
{
  char *opCodeString = instructions[0];
  u_int32_t Rd = getInt(instructions[1]) << 16;
  u_int32_t Rm = getInt(instructions[2]);
  u_int32_t Rs = getInt(instructions[3]) << 8;
  u_int32_t fillerBits = 0x90;
  if (!(strcmp(opCodeString, "mla")))
  {
    u_int32_t accFlag = 1 << 21;
    u_int32_t Rn = getInt(instructions[4]);
    Rn = Rn << 12;
    return TRUECOND + accFlag + Rd + Rn + Rm + Rs + fillerBits;
  }
  else
  {
    return TRUECOND + Rd + Rm + Rs + fillerBits;
  }
}