/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains functions which decode the instructions given by the emulator.
*/

#include "defines.h"


/*
 * SUMMARY: Returns the type of the instruction that was given in.
 *
 * PARAMETER: uint32_t bigEndianInstr - The instruction in Big Endian format.
 *
 * RETURN: INSTRUCTION
*/
INSTRUCTION workOutType(uint32_t bigEndianInstr)
{
	if (bigEndianInstr & BRANCH_INSTR_MASK)
		return BRANCH;
	else if (bigEndianInstr & TRANSFER_INSTR_MASK)
		return TRANSFER;
	else
	{
		if ((((bigEndianInstr & MULTIPLY_INSTR_MASK_1) >> BITS_IN_NIBBLE) == 0x9)
			&& !(bigEndianInstr & MULTIPLY_INSTR_MASK_2))
			return MULTIPLY;
		else
			return PROCESS;
	}
}

/*
 * SUMMARY: Converts instruction to big endian.
 *
 * PARAMETER: uint32_t byteN - contains the nth byte of the instruction in Little Endian format.
 *
 * RETURN: uint32_t
*/
uint32_t bigEndianConverter(uint32_t byte1, uint32_t byte2, uint32_t byte3, uint32_t byte4)
{
	return (byte4 << BITS_IN_THREE_BYTES) + (byte3 << BITS_IN_TWO_BYTES)
		+ (byte2 << BITS_IN_BYTE) + byte1;
}

/*
 * SUMMARY: Saves the destination register, rn and two flags (out of immediate, set condition
 * codes, accumulate, pre/post indexing) in output.
 *
 * PARAMETER: uint32_t bigEndianInstr - The instruction in Big Endian format.
 * PARAMETER: INSTRUCTION instrType - Type of instruction.
 * PARAMETER: DECODE *instrToDecode - Struct to store the decoded instruction in.
 *
 * RETURN: void
*/
void decodeHelper(uint32_t bigEndianInstr, INSTRUCTION instrType, DECODE *instrToDecode)
{
	switch (instrType)
	{
	case MULTIPLY:
		instrToDecode->destReg = (bigEndianInstr & MULTIPLY_RD_MASK) >> BITS_IN_TWO_BYTES;
		instrToDecode->rn =
			(bigEndianInstr & MULTIPLY_RN_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE);
		instrToDecode->op1 = bigEndianInstr & ACCUMULATE_MASK; // Accumulate Bit Set?
		instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
		break;
	case TRANSFER:
		instrToDecode->destReg =
			(bigEndianInstr & DATA_RD_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE);
		instrToDecode->rn = (bigEndianInstr & DATA_RN_MASK) >> BITS_IN_TWO_BYTES;
		instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Immediate if 0
		instrToDecode->op2 = bigEndianInstr & P_MASK; // Pre/Post indexing bit is set?
		break;
	case PROCESS:
		instrToDecode->destReg =
			(bigEndianInstr & DATA_RD_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE);
		instrToDecode->rn = (bigEndianInstr & DATA_RN_MASK) >> BITS_IN_TWO_BYTES;
		instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Immediate if 1
		instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
		break;
	default: // Branch instruction or Invalid instruction
		break;
	}
}

/*
 * SUMMARY: Emulator decoder used to decode instructions.
 *
 * PARAMETER: DECODE *instrToDecode - A struct to store the decoded instruction in.
 * PARAMETER: const uint32_t *fetchedInstr - Array that stores the instruction in Little Endian
 * format.
 *
 * RETURN: void
*/
void decode(const uint32_t *fetchedInstr, DECODE *instrToDecode)
{
	uint32_t byte1 = fetchedInstr[0], byte2 = fetchedInstr[1], byte3 = fetchedInstr[2],
		byte4 = fetchedInstr[3];
	instrToDecode->bigEndianInstr = bigEndianConverter(byte1, byte2, byte3, byte4);
	instrToDecode->instruction = workOutType(instrToDecode->bigEndianInstr);
	decodeHelper(instrToDecode->bigEndianInstr, instrToDecode->instruction, instrToDecode);
}

/*
 * SUMMARY: Checks if the Condition Field of the instruction is satisfied by CPSR.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: uint32_t bigEndianInstr - The instruction in Big Endian format.
 *
 * RETURN: bool - true if satisfied otherwise false.
*/
bool conditionChecker(uint32_t bigEndianInstr, ARMSTATE *state)
{
	switch (bigEndianInstr & CODE_MASK)
	{
	case 0x00000000: // EQ
		return (state->regs[CPSR_LOCATION] & Z_FLAG_MASK);
	case 0x10000000: // NE
		return !(state->regs[CPSR_LOCATION] & Z_FLAG_MASK);
	case 0xA0000000: // GE
		return (state->regs[CPSR_LOCATION] & N_FLAG_MASK)
			== (state->regs[CPSR_LOCATION] & V_FLAG_MASK);
	case 0xB0000000: // LT
		return (state->regs[CPSR_LOCATION] & N_FLAG_MASK)
			!= (state->regs[CPSR_LOCATION] & V_FLAG_MASK);
	case 0xC0000000: // GT
		return (!(state->regs[CPSR_LOCATION] & Z_FLAG_MASK))
			&& ((state->regs[CPSR_LOCATION] & N_FLAG_MASK)
				== (state->regs[CPSR_LOCATION] & V_FLAG_MASK));
	case 0xD0000000: // LE
		return (state->regs[CPSR_LOCATION] & Z_FLAG_MASK)
			|| ((state->regs[CPSR_LOCATION] & N_FLAG_MASK)
				!= (state->regs[CPSR_LOCATION] & V_FLAG_MASK));
	default: // AL
		return true;
	}
}