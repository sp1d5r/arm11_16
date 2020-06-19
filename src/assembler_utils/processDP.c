/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing the function that produces binary for a dp instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: The function which is responsible for converting a dp instruction to binary.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: uint32_t - The 32 bit integer equivalent of the dp instruction.
*/
uint32_t convertDpToBinary(instruction *state, const int currentLine)
{
  char **instructions = splitUp(state->lines[currentLine]);
  uint32_t opCode;
  uint32_t setCond = 0;
  uint32_t Rd = getInt(instructions[1]);
  Rd = Rd << 12;
  opCode = state->u.opCode;

  if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3 || opCode == 4 || opCode == 12)
  {
    // add, eor, sub, rsb, add, orr
    uint32_t Rn = (getInt(instructions[2])) << 16;
    uint32_t immBit = 0;
    uint32_t operand2 = processOperand2(&instructions[3]);
    if (instructions[3][0] == '#')
    {
      immBit = 1 << 25;
    }
    free(instructions);
    return TRUE_CONDITION + immBit + (opCode << 21) + setCond + Rn + Rd + operand2;
  }
  else if (opCode == 13)
  {
    //mov
    uint32_t immBit = 0;
    uint32_t operand2 = processOperand2(&instructions[2]);
    if (instructions[2][0] == '#')
    {
      immBit = 1 << 25;
    }
    free(instructions);
    return TRUE_CONDITION + immBit + (opCode << 21) + setCond + Rd + operand2;
  }
  else
  {
    //tst, teq, cmp
    setCond = 1 << 20;
    uint32_t immBit = 0;
    uint32_t operand2 = processOperand2(&instructions[2]);
    Rd <<= 4;

    if (instructions[2][0] == '#')
    {
      immBit = 1 << 25;
    }
    free(instructions);
    return TRUE_CONDITION + immBit + (opCode << 21) + setCond + Rd + operand2;
  }
}