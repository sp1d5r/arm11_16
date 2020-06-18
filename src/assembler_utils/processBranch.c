/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing the function that produces binary for a branch instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: The function which is responsible for converting a branch instruction to binary.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: uint32_t - The 32 bit integer equivalent of the branch instruction.
*/
uint32_t convertBranchToBinary(instruction *instr, const int currentLine) {
  char **instructions = splitUp(instr->lines[currentLine]);
  uint32_t code = instr->u.condCode;
  int labelAddress = returnAddressFromSymbolTable(instructions[1], instr->symbolTable);
  int offset;
  if (labelAddress == -1) {
	offset = getInt(instructions[1]) - (currentLine) * 4 - 8;
  } else {
	offset = labelAddress - (currentLine) * 4 - 8;

	int labelNum = 0;
	if (offset < 0) {
	  labelNum += instr->symbolTable.numberOfItems;
	}
	offset &= 0x3FFFFFF;
	offset >>= 2;
	offset &= 0xFFFFFF;
	offset += labelNum;
  }
  uint32_t fillerBits = 0x0a000000;
  code = code << 28;
  free(instructions);
  return code + fillerBits + (uint32_t)offset;
}