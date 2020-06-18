/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing the function that produces binary for a multiply instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: The function which is responsible for converting a multiplication instruction to binary.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: uint32_t - The 32 bit integer equivalent of the multiplication instruction.
*/
uint32_t convertMultiplyToBinary(instruction *state, const int currentLine) {
  char **instructions = splitUp(state->lines[currentLine]);
  uint32_t Rd = getInt(instructions[1]) << 16;
  uint32_t Rm = getInt(instructions[2]);
  uint32_t Rs = getInt(instructions[3]) << 8;
  uint32_t fillerBits = 0x90;
  if (!strcmp(instructions[0], "mla")) {
	uint32_t accFlag = 1 << 21;
	uint32_t Rn = getInt(instructions[4]);
	Rn <<= 12;
	free(instructions);
	return TRUE_CONDITION + accFlag + Rd + Rn + Rm + Rs + fillerBits;
  } else {
	free(instructions);
	return TRUE_CONDITION + Rd + Rm + Rs + fillerBits;
  }
}