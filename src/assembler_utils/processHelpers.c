#include "assemble.h"

/*
 * createStringArray
 * Params - array of strings, length of the array, the max size of each string
 */
void createStringArray(char **array, int length, int maxSize)
{
  for (int i = 0; i < length; i++)
  {
    array[i] = (char *)calloc(maxSize, sizeof(char));
  }
}

/*
 * operandTotal
 * Params - array of strings
 * Returns - the number of strings in the array (number of operands)
 */
int operandTotal(char **array)
{
  int i = 0;
  while (strcmp(array[i], "end"))
  {
    i++;
  }
  return i;
}

/*
 * getInt
 * Params - Takes a string values
 * Returns - the integer equivalent of the values
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
 * returnAddressFromSymbolTable
 * Params - label string, the current SymbTable
 * Returns - if the label is in table then return the address associated otherwise return -1
 */
int returnAddressFromSymbolTable(char *stringTarget, SymbTable lookUp)
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

u_int32_t getShiftNum(char *shift)
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

u_int32_t getShiftAmount(char **shiftOperand)
{
  u_int32_t shiftType = getShiftNum(shiftOperand[0]);
  u_int32_t shiftSpec = getInt(shiftOperand[1]);
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

u_int32_t processOffset(int offset)
{
  u_int32_t origOffset;
  int change8BitOffset = 1;
  if (offset == (u_int8_t)offset)
  {
    return (u_int32_t)offset;
  }
  else
  {
    //TODO - FIND ROTATE RIGHT AMOUNT
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
    return (u_int32_t)(rotateAmount << 8) + (u_int8_t)((origOffset & 0xFF000000) >> 24);
  }
}

u_int32_t processOperand2(char **operand2)
{
  if (operand2[0][0] == '#')
  {
    u_int32_t offset = processOffset(getInt(operand2[0]));
    return offset;
  }
  else
  {
    int length = operandTotal(operand2);
    u_int32_t rM = getInt(operand2[0]);
    if (length == 3)
    {
      u_int32_t shiftAmount = getShiftAmount(&operand2[1]);
      rM += shiftAmount;
    }
    return rM;
  }
}