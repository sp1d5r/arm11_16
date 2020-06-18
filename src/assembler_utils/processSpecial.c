/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing the function that produces binary for a special instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: The function which is responsible for converting a special instruction to binary.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: uint32_t - The 32 bit integer equivalent of the special instruction.
*/
uint32_t convertSpecialToBinary(instruction *state, const int currentLine)
{
	char **instructions = splitUp(state->lines[currentLine]);
	if (!strcmp(instructions[0], "andeq"))
	{
		return 0;
	}
	char rn[5], expression[511];
	strcpy(rn, instructions[1]);
	strcpy(expression, instructions[2]);
	sprintf(state->lines[currentLine], "mov %s,%s,lsl %s end", rn, rn, expression);
	state->u.opCode = 13;
	free(instructions);
	return convertDpToBinary(state, currentLine);
}