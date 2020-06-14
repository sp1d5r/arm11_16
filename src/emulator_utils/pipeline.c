/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains functions responsible for the pipeline.
*/

#include "pipelineConstants.h"
#include "emulateConstants.h"

/*
 * SUMMARY: Fetches the next instruction. Also sets the fetched flag in the state true
 * which will tell the pipeline that an instruction to be decoded has been fetched.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: uint32_t *fetched - A pointer which will store the fetched instruction.
 *
 * RETURN: void
*/

void fetch(ARMSTATE *state, uint32_t *fetched)
{
  fetched[0] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION]];
  fetched[1] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 1];
  fetched[2] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 2];
  fetched[3] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 3];
  state->fetchDecodeExecute[FETCHED] = true;
}

/*
 * SUMMARY: An instruction is 4 bytes. The pipeline takes an instruction and decodes it
 * while simultaneously executing the previously decoded instruction. The PC is always 8
 * bytes greater than the address of the instruction being executed.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine. Should be freshly initialised
 * when given in.
 *
 * RETURN: void
*/

void pipeline(ARMSTATE *state)
{
  uint32_t fetchedInstr[NUMBER_OF_BYTES_IN_INSTR];
  DECODE previouslyDecodedInstruction =
      {INIT_ZERO_VAL, NONE, INIT_ZERO_VAL, INIT_ZERO_VAL, false, false};
  bool firstDecode = true;
  while (true)
  {
    if (state->fetchDecodeExecute[DECODED])
      execute(&previouslyDecodedInstruction, state);
    if (!previouslyDecodedInstruction.bigEndianInstr && !firstDecode)
      break;
    if (state->fetchDecodeExecute[FETCHED])
    {
      decode(fetchedInstr,
             &previouslyDecodedInstruction);
      state->fetchDecodeExecute[DECODED] = true;
      if (firstDecode)
        firstDecode = false;
    }
    fetch(state, fetchedInstr);
    state->regs[PROGRAM_COUNTER_LOCATION] += NUMBER_OF_BYTES_IN_INSTR;
  }
  outputState(state);
}