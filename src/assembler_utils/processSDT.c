#include "assemble.h"
#include "processHelpers.h"


// datastructure
// takes instruction where it made
// the value it stores
int getSize(int *values)
{
    int i = 0;
    while (*(values + i) >= 0)
    {
        i++;
    }
    return i;
}

void updateInts(int *values, int value)
{
    values = realloc(values, (getSize(values) + 2) * sizeof(int));
    int i = 0;
    while (values[i] >= 0)
    {
        i++;
    }
    *(values + i) = value;
    *(values + i + 1) = -25;
}

/* 
calculatePFlag


*/
int calculatePFlag(char *instruction){
  int pFlag = 0;
  if (strchr(instruction, ']') == NULL)
  {
     pFlag = 1;
  }
  return pFlag;
}

u_int32_t convertSDTToBinary(char **instructions, int current_instruction, int *total_instructions, int *finalNumbers, int noOfLabels)
{
  // Initialise fillerbits and bits that don't need calculation 
  u_int32_t fillerBits = 0x04000000;
  char *loadStore = instructions[0];
  u_int32_t Rd = getInt(instructions[1]) << 12;
  int addressCount = operandTotal(instructions) - 2;
  u_int32_t loadFlag = 0;

  // calculate the load flag?
  if (!(strcmp(loadStore, "ldr")))
    {
      loadFlag = 1 << 20;
    }

  // calculate PFlag
  u_int32_t pFlag;
  if (addressCount >1)
    {
      pFlag = calculatePFlag(instructions[2]) << 24 ;
    }
    
  if (addressCount == 1)
    {
      // do case for numeric constant i.e. <=expression>
      if (instructions[2][0] == '=')
        {
	  // get the value inside the numeric expression 
	  int numericExpression; 
	  numericExpression = getInt(&(instructions[2])[1]);
	    
	  if (numericExpression < 0xFF)
            {
	      // create a move instruction
	      char **movInstruction = (char **)calloc(3, sizeof(char *));
	      createStringArray(movInstruction, 3, 10);
	      movInstruction[0] = "mov";
	      movInstruction[1] = instructions[1];
	      instructions[2][0] = '#';
	      movInstruction[2] = instructions[2];
	      u_int32_t returnValue = convertDPToBinary(movInstruction);
	      return returnValue;
            }
	  else
            {
	      char **ldrInstruction = (char **)calloc(5, sizeof(char *));
	      createStringArray(ldrInstruction, 5, 10);
	      ldrInstruction[0] = "ldr";
	      ldrInstruction[1] = instructions[1];
	      ldrInstruction[2] = "[r15";
	      
	      // calculate the offset and set it 
	      
	      int offset = *total_instructions - current_instruction - 8 - noOfLabels;
	      *total_instructions = *total_instructions + 4;
	      updateInts(finalNumbers, numericExpression);
	      char str[10];
	      sprintf(str, "=0x%04x]", offset);
	      ldrInstruction[3] = str;
	      ldrInstruction[4] = "end";

	      u_int32_t return_value = convertSDTToBinary(ldrInstruction, current_instruction, total_instructions, finalNumbers, noOfLabels);
	      return return_value;
            }
        }
      else
        {
	  // case for register value 
	  u_int32_t Rn = getInt(instructions[2]) << 16;
	  u_int32_t pFlag = 1 << 24;
	  u_int32_t uBit = 1 << 23;
	  u_int32_t return_value = TRUECOND + pFlag + uBit + fillerBits + loadFlag + Rn + Rd;
	  return return_value;
        }
    }
    else if (addressCount == 2)
    {
      // case for two registers 
      u_int uBit = 1 << 23;
      
      // calculate I flag 
      int iFlag = 0;
      
      if (instructions[3][0] == 'r')
        {
	  iFlag = 1 << 25;
        }

      u_int32_t Rn = 0;
      u_int32_t offset = 0;
      Rn = getInt(instructions[2]) << 16;
      offset = getInt(&(instructions[3])[1]);
	
      return TRUECOND + fillerBits + iFlag + pFlag + uBit + loadFlag + Rd + Rn + offset;
    }
    else if (addressCount == 3)
    {
      // case for minus constant

      // check if I flag needs to be set:
      int iFlag = 0;
      if (instructions[4][0] == 'r')
        {
	  iFlag = 1 << 25;
        }
      
      u_int32_t Rn = 0;
      u_int32_t offset = 0;
      Rn = getInt(instructions[2]) << 16;
      offset = getInt(instructions[4]);
       
      return TRUECOND + fillerBits + iFlag + pFlag + loadFlag + Rd + Rn + offset;
    }
    else
    {
      // case for special instructions 
      u_int uBit = 1 << 23;

      // check if I flag needs to be set:
      int iFlag = 0;
      iFlag = 1 << 25;

      u_int32_t Rn = 0;
      Rn = getInt(instructions[2]) << 16;
      u_int32_t offset = processOperand2(&(instructions)[3]);

      return TRUECOND + fillerBits + uBit + iFlag + pFlag + loadFlag + Rd + Rn + offset;
    }
    return EXIT_FAILURE;
}