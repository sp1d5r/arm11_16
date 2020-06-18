/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains functions responsible for printing after all instructions have been
 * executed.
*/

#include "emulateConstants.h"

/*
 * SUMMARY: Gets the value stored in memory from the given address.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: int addr - The address from which to obtain memory.
 *
 * RETURN: uint32_t
*/

uint32_t getMem(ARMSTATE *state, int addr)
{
	return (state->memory[addr] << 24) | (state->memory[addr + 1] << 16)
		| (state->memory[addr + 2] << 8) | state->memory[addr + 3];
}

/*
 * SUMMARY: Outputs all Non Zero Memory after the program has ended.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void outputNonZeroMem(ARMSTATE *state)
{
	printf("Non-zero memory:\n");
	for (int i = 0; i < MAX_BYTES; i += NUMBER_OF_BYTES_IN_INSTR)
	{
		uint32_t instr = getMem(state, i);
		if (instr)
			printf("0x%08x: 0x%08x\n", i, instr);
	}
}

/*
 * SUMMARY: Outputs the state of registers after the program has ended.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void outputState(ARMSTATE *state)
{
	uint32_t regState, i;
	printf("Registers:\n");
	for (i = 0; i < NUMBER_OF_REGISTERS - 4; ++i)
	{
		regState = state->regs[i];
		printf("$%-3d: %10d (0x%08x)\n", i, regState, regState);
	}
	regState = state->regs[PROGRAM_COUNTER_LOCATION];
	printf("PC  : %10d (0x%08x)\n", regState, regState);
	regState = state->regs[CPSR_LOCATION];
	printf("CPSR: %10d (0x%08x)\n", regState, regState);
	outputNonZeroMem(state);
}