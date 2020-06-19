/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing helpers for processing the instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: Returns the total number of operands.
 *
 * PARAMETER: char **array - Array of strings.
 *
 * RETURN: int - The number of strings in the array (number of operands)
*/
int operandTotal(char **array)
{
  int i = 0;
  while (strcmp(array[i], "end") != 0)
  {
    i++;
  }
  return i;
}

/*
 * SUMMARY: Gets the integer equivalent of string values,
 *
 * PARAMETER: char *values - String.
 *
 * RETURN: int - The integer equivalent of the values
*/
int getInt(char *values)
{
  if (values[0] == '[' || values[0] == '=' || values[0] == '#')
  {
    return getInt(values + 1);
  }
  else if (values[0] == 'r')
  {
    return atoi(values + 1);
  }
  else if (!(values[0] == '0' && values[1] == 'x'))
  {
    return atoi(values);
  }
  else
  {
    return (int)strtol((values + 2), NULL, 16);
  }
}

/*
 * SUMMARY: Returns the address associated with a label.
 *
 * PARAMETER: char *stringTarget - The label whose address is to be returned.
 * PARAMETER: symbolTable lookup - The symbol table containing the labels and their addresses.
 *
 * RETURN: int - If the label is in the table then return the address associated otherwise return -1.
*/
int returnAddressFromSymbolTable(char *stringTarget, symbolTable lookUp)
{
  if (lookUp.numberOfItems == 0)
  {
    return -1;
  }
  for (int i = 0; i < lookUp.numberOfItems; i++)
  {
    if (!(strcmp(lookUp.labels[i], stringTarget)))
    {
      return lookUp.memoryAddresses[i];
    }
  }
  return -1;
}

/*
 * SUMMARY: Gets the shift number.
 *
 * PARAMETER: char *shift - The shift type.
 *
 * RETURN: int - The shift number.
*/
int getShiftNum(char *shift)
{
  if (strcmp("lsl", shift) == 0)
  {
    return 0;
  }
  else if (strcmp("lsr", shift) == 0)
  {
    return 1;
  }
  else if (strcmp("asr", shift) == 0)
  {
    return 2;
  }
  else if (strcmp("ror", shift) == 0)
  {
    return 3;
  }
  exit(EXIT_FAILURE);
}

/*
 * SUMMARY: Gets the shift amount.
 *
 * PARAMETER: char **shiftOperand - The shift operand.
 *
 * RETURN: uint32_t - The shift amount.
*/
uint32_t getShiftAmount(char **shiftOperand)
{
  uint32_t shiftType = getShiftNum(shiftOperand[0]);
  uint32_t shiftSpec = getInt(shiftOperand[1]);
  if (shiftOperand[1][0] == '#')
  {
    shiftSpec <<= 7;
  }
  else
  {
    shiftSpec <<= 8;
    shiftSpec |= 16;
  }
  shiftType <<= 5;
  return shiftSpec + shiftType;
}

/*
 * SUMMARY: Processes the offset.
 *
 * PARAMETER: int offset - The offset
 *
 * RETURN: uint32_t - The processed offset.
*/
uint32_t processOffset(int offset)
{
  uint32_t origOffset;
  int change8BitOffset = 1;
  if (offset == (uint8_t)offset)
  {
    return (uint32_t)offset;
  }
  else
  {
    int rotateAmount = 0;
    while (offset != 0)
    {

      if (((offset & 0x00FFFFFF) == 0) && change8BitOffset)
      {
        origOffset = offset;
        change8BitOffset = 0;
      }
      offset <<= 2;
      rotateAmount++;
    }
    if (rotateAmount >= 16)
    {
      perror("Can't represent number in 12 Bit");
      exit(EXIT_FAILURE);
    }
    return (uint32_t)(rotateAmount << 8) + (uint8_t)((origOffset & 0xFF000000) >> 24);
  }
}

/*
 * SUMMARY: Processes operand2.
 *
 * PARAMETER: char ** - An array of strings representing operand2.
 *
 * RETURN: uint32_t - The processed operand2.
*/
uint32_t processOperand2(char **operand2)
{
  if (operand2[0][0] == '#')
  {
    uint32_t offset = processOffset(getInt(operand2[0]));
    return offset;
  }
  else
  {
    int length = operandTotal(operand2);
    uint32_t rM = getInt(operand2[0]);
    if (length == 3)
    {
      uint32_t shiftAmount = getShiftAmount(&operand2[1]);
      rM += shiftAmount;
    }
    return rM;
  }
}