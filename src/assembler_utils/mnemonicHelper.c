#include "assemble.h"

/*
 * getMnemonic
 * Params - array of strings (split up arm instructions)
 * Returns - the enum MNEMONIC of the instruction
 */
MNEMONICS getMnemonic(char **instruction)
{
  char *opcode = instruction[0];
  if (!strcmp(opcode, "add"))
  {
    return ADD;
  }
  else if (!strcmp(opcode, "sub"))
  {
    return SUB;
  }
  else if (!strcmp(opcode, "rsb"))
  {
    return RSB;
  }
  else if (!strcmp(opcode, "and"))
  {
    return AND;
  }
  else if (!strcmp(opcode, "eor"))
  {
    return EOR;
  }
  else if (!strcmp(opcode, "orr"))
  {
    return ORR;
  }
  else if (!strcmp(opcode, "mov"))
  {
    return MOV;
  }
  else if (!strcmp(opcode, "tst"))
  {
    return TST;
  }
  else if (!strcmp(opcode, "teq"))
  {
    return TEQ;
  }
  else if (!strcmp(opcode, "cmp"))
  {
    return CMP;
  }
  else if (!strcmp(opcode, "mul"))
  {
    return MUL;
  }
  else if (!strcmp(opcode, "mla"))
  {
    return MLA;
  }
  else if (!strcmp(opcode, "ldr"))
  {
    return LDR;
  }
  else if (!strcmp(opcode, "str"))
  {
    return STR;
  }
  else if (!strcmp(opcode, "beq"))
  {
    return BEQ;
  }
  else if (!strcmp(opcode, "bne"))
  {
    return BNE;
  }
  else if (!strcmp(opcode, "bge"))
  {
    return BGE;
  }
  else if (!strcmp(opcode, "blt"))
  {
    return BLT;
  }
  else if (!strcmp(opcode, "bgt"))
  {
    return BGT;
  }
  else if (!strcmp(opcode, "b"))
  {
    return B;
  }
  else if (!strcmp(opcode, "ble"))
  {
    return BLE;
  }
  else if (!strcmp(opcode, "andeq"))
  {
    return ANDEQ;
  }
  else if (!strcmp(opcode, "lsl"))
  {
    return LSL;
  }
  else
  {
    return LABEL;
  }
}