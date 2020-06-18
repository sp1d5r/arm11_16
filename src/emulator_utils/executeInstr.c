/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains the function responsible for executing a decoded instruction.
*/

#include "executeConstants.h"
#include "emulateConstants.h"

/*
 * SUMMARY: Emulator executor used to execute instructions.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: DECODE *executableInstr - A pointer to the struct which stores 
 * properties of the decoded instruction such as the destination register.
 *
 * RETURN: void
*/

void execute(DECODE *executableInstr, ARMSTATE *state)
{
	if (conditionChecker(executableInstr->bigEndianInstr, state))
	{
		switch (executableInstr->instruction)
		{
		case MULTIPLY:
			multiply(executableInstr, state);
			break;
		case BRANCH:
			branch(executableInstr, state);
			break;
		case PROCESS:
			process(executableInstr, state);
			break;
		case TRANSFER:
			transfer(executableInstr, state);
			break;
		case NONE:
			printf("INVALID INSTRUCTION ENTERED!");
		}
	}
}
