/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The file containing the function that produces binary for an sdt instruction.
*/

#include "assemble.h"

/*
 * SUMMARY: Updates the finalInts malloc.
 *
 * PARAMETER: int *values - The malloc.
 * PARAMETER: int value - The value to be stored.
 * PARAMETER: int size - The size of finalInts malloc.
 *
 * RETURN: void
*/
void updateInts(int *values, int value, int size)
{
	values = realloc(values, (size + 1) * sizeof(int));
	CHECK_IF_NULL(values)
	int i = 0;
	while (values[i] >= 0)
	{
		i++;
	}
	*(values + i) = value;
	*(values + i + 1) = -25;
}

/*
 * SUMMARY: Calculates the p flag.
 *
 * PARAMETER: char *instruction - The current instruction.
 *
 * RETURN: int - The p flag.
*/
int calculatePFlag(char *instruction)
{
	int pFlag = 0;
	if (strchr(instruction, ']') == NULL)
	{
		pFlag = 1;
	}
	return pFlag;
}

/*
 * SUMMARY: The function which is responsible for converting a sdt instruction to binary.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: uint32_t - The 32 bit integer equivalent of the sdt instruction.
*/
uint32_t convertSdtToBinary(instruction *state, const int currentLine)
{
	char **instructions = splitUp(state->lines[currentLine]);
	// Initialise filler bits and bits that don't need calculation
	uint32_t fillerBits = 0x04000000;
	char *loadStore = instructions[0];
	uint32_t Rd = getInt(instructions[1]) << 12;
	int addressCount = operandTotal(instructions) - 2;
	uint32_t loadFlag = 0;
	// calculate the load flag?
	if (!(strcmp(loadStore, "ldr")))
	{
		loadFlag = 1 << 20;
	}
	// calculate PFlag
	uint32_t pFlag;
	if (addressCount > 1)
	{
		pFlag = calculatePFlag(instructions[2]) << 24;
	}
	if (addressCount == 1)
	{
		// do case for numeric constant currentLine.e. <=expression>
		if (instructions[2][0] == '=')
		{
			// get the value inside the numeric expression
			int numericExpression;
			numericExpression = getInt(&(instructions[2])[1]);
			if (numericExpression < 0xFF)
			{
				// create a move instruction
				instructions[2][0] = '#';
				char expression[511], rn[511];
				strcpy(rn, instructions[1]);
				strcpy(expression, instructions[2]);
				sprintf(state->lines[currentLine], "mov %s,%s end", rn, expression);
				state->u.opCode = 13;
				free(instructions);
				return convertDpToBinary(state, currentLine);
			}
			else
			{
				char expression[511], rn[511];
				strcpy(rn, instructions[1]);
				strcpy(expression, instructions[2]);
				// calculate the offset and set it
				int offset = state->sdt_helper.no_instructions - (currentLine) * 4 - 8
					- 4 * state->symbolTable.numberOfItems;
				state->sdt_helper.no_instructions += 4;
				updateInts(state->sdt_helper.finalNumbers,
					numericExpression,
					state->sdt_helper.sizeOfFinalNumbers);
				state->sdt_helper.sizeOfFinalNumbers++;
				char str[10];
				sprintf(str, "=0x%04x]", offset);
				sprintf(state->lines[currentLine], "ldr %s,[r15 =0x%04x] end", rn, offset);
				free(instructions);
				return convertSdtToBinary(state, currentLine);
			}
		}
		else
		{
			// case for register value
			uint32_t Rn = getInt(instructions[2]) << 16;
			pFlag = 1 << 24;
			uint32_t uBit = 1 << 23;
			free(instructions);
			return TRUE_CONDITION + pFlag + uBit + fillerBits + loadFlag + Rn + Rd;
		}
	}
	else if (addressCount == 2)
	{
		// case for two registers
		int uBit = 1 << 23;
		// calculate I flag
		int iFlag = 0;
		if (instructions[3][0] == 'r')
		{
			iFlag = 1 << 25;
		}
		uint32_t Rn;
		uint32_t offset;
		Rn = getInt(instructions[2]) << 16;
		offset = getInt(&(instructions[3])[1]);
		free(instructions);
		return TRUE_CONDITION + fillerBits + iFlag + pFlag + uBit + loadFlag + Rd + Rn + offset;
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
		uint32_t Rn;
		uint32_t offset;
		Rn = getInt(instructions[2]) << 16;
		offset = getInt(instructions[4]);
		free(instructions);
		return TRUE_CONDITION + fillerBits + iFlag + pFlag + loadFlag + Rd + Rn + offset;
	}
	else
	{
		// case for special instructions
		int uBit = 1 << 23;
		// check if I flag needs to be set:
		int iFlag;
		iFlag = 1 << 25;
		uint32_t Rn;
		Rn = getInt(instructions[2]) << 16;
		uint32_t offset = processOperand2(&(instructions)[3]);
		free(instructions);
		return TRUE_CONDITION + fillerBits + uBit + iFlag + pFlag + loadFlag + Rd + Rn + offset;
	}
}