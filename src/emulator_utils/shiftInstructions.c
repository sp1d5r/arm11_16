/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains common shift functions used in data transfer and data process instructions
*/

#include "emulateConstants.h"

/*
 * SUMMARY: The rotate right operation.
 *
 * PARAMETER: uint32_t operand - The operand given in the instruction.
 * PARAMETER: uint8_t rotateAmount - Amount to rotate by.
 *
 * RETURN: uint32_t
*/

uint32_t rotateRight(uint32_t operand, uint8_t rotateAmount)
{
  uint32_t shift = operand >> rotateAmount;
  uint32_t cycle = operand << (32 - rotateAmount);
  return shift | cycle;
}

/*
 * SUMMARY: Returns shift type
 *
 * PARAMETER: uint16_t operand - The operand given in the instruction.
 *
 * RETURN: SHIFTTYPE
*/

SHIFTTYPE shiftType(uint16_t operand)
{
  int typeOpCode = (operand & TYPEMASK) >> 5;
  switch (typeOpCode)
  {
  case 1:
    return LOGICALRIGHT;
  case 2:
    return ARITHMETICRIGHT;
  case 3:
    return ROTATERIGHT;
  default:
    return LOGICALLEFT;
  }
}

/*
 * SUMMARY: Shifts valueInRm by shiftAmount based on the type of shift.
 *
 * PARAMETER: uint32_t *valueInRm - pointer to the value in Rm.
 * PARAMETER: SHIFTTYPE type - Type of shifting specified by the instruction.
 * PARAMETER: uint8_t shiftAmount - Amount to shift by.
 *
 * RETURN: bool
*/

bool shift(uint32_t *valueInRm, SHIFTTYPE type, uint8_t shiftAmount)
{
  bool carry;
  switch (type)
  {
  case LOGICALLEFT:
    carry = (*valueInRm >> (31 - shiftAmount)) & 0x1;
    *valueInRm <<= shiftAmount;
    break;
  case LOGICALRIGHT:
    carry = (*valueInRm >> (shiftAmount - 1)) & 0x1;
    *valueInRm >>= shiftAmount;
    break;
  case ARITHMETICRIGHT:
    carry = (*valueInRm >> (shiftAmount - 1)) & 0x1;
    bool signBit = *valueInRm & 0x80000000;
    for (int i = 0; i < shiftAmount; i++)
    {
      *valueInRm >>= 1;
      if (signBit)
        *valueInRm |= 0x80000000;
    }
    break;
  case ROTATERIGHT:
    carry = (*valueInRm >> (shiftAmount - 1)) & 0x1;
    *valueInRm = rotateRight(*valueInRm, shiftAmount);
    break;
  default:
    printf("Unknown instruction entered");
  }
  return carry;
}

/*
 * SUMMARY: Shifts a register for a transfer/process instruction.
 *
 * PARAMETER: ARMSTATE *state - Current state of the machine.
 * PARAMETER: uint16_t operand - Operand given by the instruction.
 * PARAMETER: bool *shiftCarry - Pointer to the carry flag which will 
 * store the carry bit obtained by doing shift operations.
 * PARAMETER: uint8_t amount - Amount to shift by.
 *
 * RETURN: uint32_t
*/

uint32_t shiftRegister(ARMSTATE *state, uint8_t amount, uint16_t operand, bool *shiftCarry)
{
  uint32_t valueInRm = state->regs[operand & 0x000F];
  SHIFTTYPE type = shiftType(operand);
  *shiftCarry = shift(&valueInRm, type, amount);
  return valueInRm;
}