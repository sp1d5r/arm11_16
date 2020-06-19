/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains the functions responsible for executing a decoded instruction.
*/

#include "executeConstants.h"
#include "emulateConstants.h"

// -----------------------------------
// BRANCH INSTRUCTION STARTS
// -----------------------------------

/*
 * SUMMARY: The branch function calculates the shifted offset, sign extends it and then adds it
 * onto the program counter. It also makes the FETCHED and DECODED flags zero in order to ensure
 * that decoding is not done before fetching the next instruction and execution is not done
 * before decoding the fetched instruction.
 *
 * PARAMETER: DECODE *executableInstr - A pointer to the decoded instruction which holds the 
 * 'bigEndianInstr'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void branch(DECODE *executableInstr, ARMSTATE *state)
{
  int32_t shiftedOffset = (executableInstr->bigEndianInstr & OFFSET_MASK) << OFFSET_SHIFT;
  // If shiftedOffset is negative, then sign extend with 1s.
  if (shiftedOffset & SIGN_BIT_MASK)
    state->regs[PROGRAM_COUNTER_LOCATION] += (shiftedOffset | SIGN_EXTEND_MASK);
  else
    state->regs[PROGRAM_COUNTER_LOCATION] += shiftedOffset;
  state->fetchDecodeExecute[FETCHED] = state->fetchDecodeExecute[DECODED] = false;
}

// -----------------------------------
// MULTIPLY INSTRUCTION STARTS
// -----------------------------------

/*
 * SUMMARY: Rd:=Rm×Rs if accumulate bit is set otherwise Rd:=Rm×Rs+Rn.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * , 'Rd' and 'Rn'. It also holds the value of the 'accumulate' and 'set condition codes' flag.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

// Could have also done Rd:=Rm×Rs+(Rn×accumulate) but that would have required an extra add instruction.
void multiply(DECODE *executableInstr, ARMSTATE *state)
{
  uint32_t rm = state->regs[executableInstr->bigEndianInstr & MULTIPLY_RM_MASK],
           rs = state->regs[(executableInstr->bigEndianInstr & MULTIPLY_RS_MASK) >> BITS_IN_BYTE];
  uint32_t result = rm * rs;
  if (executableInstr->op1) // -- op1 is the 'accumulate' flag.
  {
    uint32_t rn = state->regs[executableInstr->rn];
    result += rn;
  }
  state->regs[executableInstr->destReg] = result;
  if (executableInstr->op2) // -- op2 is the 'set condition codes' flag.
  {
    uint32_t NF = result & N_FLAG_MASK;
    uint32_t ZF = !((bool)result) << 30;
    state->regs[CPSR_LOCATION] = NF + ZF + (state->regs[CPSR_LOCATION] & Z_AND_N_FLAG_MASK);
  }
}

// -----------------------------------
// DATA PROCESS INSTRUCTION STARTS
// -----------------------------------

/*
 * SUMMARY: Returns the opCode stored in the instruction.
 *
 * PARAMETER: uint32_t bigEndianInstr - The instruction in Big Endian format.
 *
 * RETURN: uint8_t
*/

uint8_t getOpCode(uint32_t bigEndianInstr)
{
  uint32_t opCode = bigEndianInstr >> 21;
  return opCode & OPCODE_MASK;
}

/*
 * SUMMARY: Determines whether the immediate flag is set and processes (and returns) operand2
 * accordingly.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: bool *shiftCarry - Pointer to the carry bit returned from the shift instruction.
 *
 * RETURN: uint32_t
*/

uint32_t processHelper(DECODE *executableInstr, ARMSTATE *state, bool *shiftCarry)
{
  uint16_t operand2 = executableInstr->bigEndianInstr & 0x00000FFF;
  if (executableInstr->op1) // -- op1 is the immediate flag.
  {
    uint8_t amount = (operand2 & 0x0F00) >> 7;
    uint32_t immVal = operand2 & 0x00FF;
    *shiftCarry = (immVal >> amount) & 0x1;
    return rotateRight(immVal, amount);
  }
  else
  {
    uint8_t amount, bit4 = operand2 >> 4 & 0x1;
    if (!bit4)
      amount = (operand2 & 0x0F80) >> 7;
    else
    {
      uint32_t rsIndex = (operand2 >> 8) & 0xF;
      uint32_t valRs = state->regs[rsIndex];
      amount = valRs & 0xF;
    }
    return shiftRegister(state, amount, operand2, shiftCarry);
  }
}

/*
 * SUMMARY: Function for the Data Processing Instructions.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * , 'Rd' and 'Rn'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void process(DECODE *executableInstr, ARMSTATE *state)
{
  uint8_t opCode = getOpCode(executableInstr->bigEndianInstr);
  uint32_t rNContent = state->regs[executableInstr->rn];
  bool CF;
  uint32_t processedOperand2 = processHelper(executableInstr, state, &CF);
  uint32_t result;
  switch ((OPCODE)opCode)
  {
  case AND:
    result = rNContent & processedOperand2;
    break;
  case EOR:
    result = rNContent ^ processedOperand2;
    break;
  case SUB:
    CF = processedOperand2 <= rNContent;
    result = rNContent - processedOperand2;
    break;
  case RSB:
    CF = rNContent <= processedOperand2;
    result = processedOperand2 - rNContent;
    break;
  case ADD:
    CF = (0xFFFFFFFF - processedOperand2) < rNContent;
    result = processedOperand2 + rNContent;
    break;
  case TST:
    result = rNContent & processedOperand2;
    break;
  case TEQ:
    result = rNContent ^ processedOperand2;
    break;
  case CMP:
    CF = processedOperand2 <= rNContent;
    result = rNContent - processedOperand2;
    break;
  case ORR:
    result = rNContent | processedOperand2;
    break;
  case MOV:
    result = processedOperand2;
  }

  if (opCode < 8 || opCode > 10) // Checks if the result should be stored.
    state->regs[executableInstr->destReg] = result;

  if (executableInstr->op2) // -- op2 is the set condition codes flag.
  {
    bool ZF = !result;
    bool NF = result & 0x80000000;
    uint32_t VF = state->regs[CPSR_LOCATION] & 0x10000000;
    state->regs[CPSR_LOCATION] = (NF << 31) + (ZF << 30) + (CF << 29) + VF;
  }
}

// -----------------------------------
// DATA TRANSFER INSTRUCTION STARTS
// -----------------------------------

/*
 * SUMMARY: Checks if the upBit is set. If it is, then offset is added to the value in rn
 * else it is subtracted from the value in rn.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * and 'Rn'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER uint32_t offset: The processed offset given in the instruction.
 *
 * RETURN: uint32_t
*/

uint32_t addOrSub(uint32_t offset, DECODE *executableInstr, ARMSTATE *state)
{
  if (executableInstr->bigEndianInstr & U_MASK)
    return state->regs[executableInstr->rn] + offset;
  else
    return state->regs[executableInstr->rn] - offset;
}

/*
 * SUMMARY: Checks if given address is out of bounds. Prints an error if it is.
 *
 * PARAMETER: uint32_t address - The address to be checked.
 *
 * RETURN: bool - true if out of bounds else false.
*/

bool outOfBounds(uint32_t address)
{
  if (address >= MAX_BYTES)
  {
    printf("Error: Out of bounds memory access at address 0x%08x\n", address);
    return true;
  }
  return false;
}

/*
 * SUMMARY: Loads data from given memory address into a register.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * and 'Rd'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: uint32_t address - The address in memory from which the value should be obtained.
 *
 * RETURN: void
*/

void load(ARMSTATE *state, DECODE *executableInstr, uint32_t address)
{
  state->regs[executableInstr->destReg] = bigEndianConverter(state->memory[address],
                                                             state->memory[address + 1],
                                                             state->memory[address + 2],
                                                             state->memory[address + 3]);
}

/*
 * SUMMARY: Stores data into memory.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * and 'Rd'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: uint32_t address - The address in memory where the data is stored.
 *
 * RETURN: void
*/

void store(ARMSTATE *state, DECODE *executableInstr, uint32_t address)
{
  uint32_t bigEndianRegValue = state->regs[executableInstr->destReg];
  state->memory[address] = bigEndianRegValue & 0x000000FF;
  state->memory[address + 1] = (bigEndianRegValue & 0x0000FF00) >> 8;
  state->memory[address + 2] = (bigEndianRegValue & 0x00FF0000) >> 16;
  state->memory[address + 3] = (bigEndianRegValue & 0xFF000000) >> 24;
}

/*
 * SUMMARY: Function that works out the address to load/store at, checks if its in bounds and
 * performs the required action.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * , 'Rd' and 'Rn'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER uint16_t offset: The processed offset given in the instruction.
 *
 * RETURN: void
*/

void transferHelper(uint32_t offset, DECODE *executableInstr, ARMSTATE *state)
{
  uint32_t address = addOrSub(offset, executableInstr, state);
  if (outOfBounds(address))
    return;
  if (executableInstr->bigEndianInstr & S_OR_L_MASK)
    load(state, executableInstr, address);
  else
    store(state, executableInstr, address);
}

/*
 * SUMMARY: Function for the Data Transfer Instructions.
 *
 * PARAMETER: DECODE *executableInstr - The decoded instruction which holds the 'bigEndianInstr'
 * , 'Rd' and 'Rn'.
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void transfer(DECODE *executableInstr, ARMSTATE *state)
{
  executableInstr->op1 = !executableInstr->op1;
  bool CF;
  uint16_t offset = executableInstr->bigEndianInstr & OFFSET_MASK_2;
  uint32_t processedOffset;
  if (executableInstr->op1) // -- op1 is the inverted immediate flag.
    processedOffset = offset;
  else
    processedOffset = shiftRegister(state, offset >> 7, offset, &CF);
  if (executableInstr->op2) // -- op2 is the pre/post indexing flag.
    transferHelper(processedOffset, executableInstr, state);
  else
  {
    transferHelper(0, executableInstr, state);
    state->regs[executableInstr->rn] = addOrSub(processedOffset, executableInstr, state);
  }
}