#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PROGRAM_COUNTER_LOCATION 13
#define NUMBER_OF_REGISTERS 15
#define MAX_BYTES 65536
#define CPSR_LOCATION 14
#define ALLOCATION_ERROR -1
#define NUMBER_OF_ARGUMENTS 2
#define BRANCH_INSTR_MASK 0x08000000
#define MULTIPLY_INSTR_1_MASK 0x000000F0
#define MULTIPLY_INSTR_2_MASK 0x0FC00000
#define TRANSFER_INSTR_MASK 0x04000000
#define DATA_RN_MASK 0x000F0000
#define DATA_RD_MASK 0x0000F000
#define IMMEDIATE_MASK 0x02000000
#define S_OR_L_MASK 0x00100000
#define U_MASK 0x00800000
#define P_MASK 0x01000000
#define MULTIPLY_RN_MASK 0x0000F000
#define MULTIPLY_RS_MASK 0x00000F00
#define MULTIPLY_RD_MASK 0x000F0000
#define MULTIPLY_RM_MASK 0x0000000F
#define ACCUMULATE_MASK 0x00200000
#define COND_FIELD_MASK 0xF0000000
#define NIBBLE 4
#define BYTE 8
#define TWO_BYTES 16
#define THREE_BYTES 24
#define INIT_ZERO_VAL 0
#define OFFSET_MASK 0x00FFFFFFFF
#define OFFSET_SHIFT 2
#define SIGN_BIT_MASK 0x02000000

// enum for the instructions available.
typedef enum INSTRUCTION {
    MULTIPLY, BRANCH, PROCESS, TRANSFER, NONE
} INSTRUCTION;

// Struct storing the current state of the machine.
typedef struct ARMState {
    uint8_t *memory;
    uint32_t regs[NUMBER_OF_REGISTERS];
} ARMState;

// Struct to store the value of the 4 bit instruction in big endian, the instruction type,
// the registers which are required and the values of any other bits such as the Immediate operand.
typedef struct DECODED {
    uint32_t bigEndianInstr;
    INSTRUCTION instruction;
    uint8_t destReg, opReg1, opReg2, opReg3;
    bool op1, op2, upBit, loadSToreBit;
} DECODED;

// Initialises all registers and memory addressed to zero.
void initialiseState(ARMState *state) {
    for (int i = 0; i < NUMBER_OF_REGISTERS; i++) {
        state->regs[i] = INIT_ZERO_VAL;
    }
    state->memory = (uint8_t *) calloc(MAX_BYTES, sizeof(uint8_t));
}

// Returns the type of instruction given in.
INSTRUCTION workOutType(uint32_t bigEndianInstr) {
    if (bigEndianInstr & BRANCH_INSTR_MASK) {
        return BRANCH;
    } else if (bigEndianInstr & TRANSFER_INSTR_MASK) {
        return TRANSFER;
    } else {
        if ((((bigEndianInstr & MULTIPLY_INSTR_1_MASK) >> NIBBLE) == 0x9) &&
            !(bigEndianInstr & MULTIPLY_INSTR_2_MASK)) {
            return MULTIPLY;
        } else {
            return PROCESS;
        }
    }
}

// Converts instruction to big endian.
uint32_t bigEndianConverter(uint32_t byte1, uint32_t byte2, uint32_t byte3, uint32_t byte4) {
    return (byte4 << THREE_BYTES) + (byte3 << TWO_BYTES) + (byte2 << BYTE) + byte1;
}

// Saves the register numbers and any sign/condition flags in output.
void decodeHelper(uint32_t bigEndianInstr, INSTRUCTION instrType, DECODED *instrToDecode) {
    switch (instrType) {
        case MULTIPLY:
            instrToDecode->destReg = (bigEndianInstr & MULTIPLY_RD_MASK) >> TWO_BYTES; // Rd
            instrToDecode->opReg1 = (bigEndianInstr & MULTIPLY_RN_MASK) >> (BYTE + NIBBLE); // Rn
            instrToDecode->opReg2 = (bigEndianInstr & MULTIPLY_RS_MASK) >> BYTE; // Rs
            instrToDecode->opReg3 = (bigEndianInstr & MULTIPLY_RM_MASK); // Rm
            instrToDecode->op1 = bigEndianInstr & ACCUMULATE_MASK; // Accumulate Bit Set?
            instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
            break;
        case TRANSFER:
            instrToDecode->destReg = (bigEndianInstr & DATA_RD_MASK) >> (BYTE + NIBBLE); // Rd
            instrToDecode->opReg1 = (bigEndianInstr & DATA_RN_MASK) >> TWO_BYTES; // Rn
            instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Operand 2 is an immediate constant if 1
            instrToDecode->op2 = bigEndianInstr & P_MASK; // Pre/Post indexing bit is set?
            instrToDecode->upBit = bigEndianInstr & U_MASK; // Up bit is set?
            instrToDecode->loadSToreBit = bigEndianInstr & S_OR_L_MASK; // Load/Store bit is set?
            break;
        case PROCESS:
            instrToDecode->destReg = (bigEndianInstr & DATA_RD_MASK) >> (BYTE + NIBBLE); // Rd
            instrToDecode->opReg1 = (bigEndianInstr & DATA_RN_MASK) >> TWO_BYTES; // Rn
            instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Operand 2 is an immediate constant if 1
            instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
            break;
        default:
            break;
    }
}

// Emulator decoder used to decode instructions.
void decode(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, DECODED *instrToDecode) {
    instrToDecode->bigEndianInstr = bigEndianConverter(byte1, byte2, byte3, byte4);
    instrToDecode->instruction = workOutType(instrToDecode->bigEndianInstr);
    decodeHelper(instrToDecode->bigEndianInstr, instrToDecode->instruction, instrToDecode);
}

// Check Condition Field
bool conditionChecker(uint32_t bigEndianInstr, ARMState *state) {
    return ((bigEndianInstr & COND_FIELD_MASK) >> 28) == state->regs[CPSR_LOCATION];
}

// Branch Instruction
void branch(DECODED *executableInstr, ARMState *state) {
    uint32_t shiftedOffset = (executableInstr->bigEndianInstr & OFFSET_MASK) << OFFSET_SHIFT;
    bool signBit = (shiftedOffset & SIGN_BIT_MASK);
    if (signBit)
        state->regs[PROGRAM_COUNTER_LOCATION] = shiftedOffset | 0xFC000000;
    else
        state->regs[PROGRAM_COUNTER_LOCATION] = shiftedOffset;
}

// Multiply Instruction
void multiply(DECODED *executableInstr, ARMState *state) {
}

// Process Instruction
void process(DECODED *executableInstr, ARMState *state) {}

// Transfer Instruction
void transfer(DECODED *executableInstr, ARMState *state) {}

// Emulator executor used to execute instructions.
void execute(DECODED *executableInstr, ARMState *state) {
    if (conditionChecker(executableInstr->bigEndianInstr, state)) {
        switch (executableInstr->instruction) {
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
                printf("An error has occurred!");
                break;
        }
    }
}

// Outputs the state of all Non Zero Memory after program has ended. 
void outputNonZeroMem(ARMState *state, int noOfInstructions) {
    printf("Non-Zero memory:");
    for (int i = 0; i < noOfInstructions; i += 4) {
        if (state->memory[i]) {
            printf("\n0x%08x: 0x%02x%02x%02x%02x", i, state->memory[i], state->memory[i + 1], state->memory[i + 2],
                   state->memory[i + 3]);
        }
    }
}

// Outputs the state of registers after program has ended.
void outputState(ARMState *state, int noOfInstructions) {
    uint32_t regState, i;
    for (i = 0; i < NUMBER_OF_REGISTERS - 2; ++i) {
        regState = state->regs[i];
        printf("$%-3d:%11d (0x%08x)\n", i, regState, regState);
    }
    printf("PC  :%11d (0x%08x)\n", state->regs[i], state->regs[i]);
    printf("CPSR:%11d (0x%08x)\n", state->regs[i + 1], state->regs[i + 1]);
    outputNonZeroMem(state, noOfInstructions);
}

// Fetches the next instruction.
void fetch(ARMState *state, uint32_t *fetched) {
    fetched[0] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION]];
    fetched[1] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 1];
    fetched[2] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 2];
    fetched[3] = state->memory[state->regs[PROGRAM_COUNTER_LOCATION] + 3];
}

// An instruction is 4 bytes. The pipeline takes an instruction and decodes it
// while simultaneously executing the previously decoded instruction.
// The PC is always 8 bytes greater than the address of the instruction being executed.
void pipe(ARMState *state, int noOfInstructions) {
    uint32_t fetchedInstr[4];
    DECODED previouslyDecodedInstruction = {INIT_ZERO_VAL, NONE, INIT_ZERO_VAL, INIT_ZERO_VAL, INIT_ZERO_VAL,
                                            INIT_ZERO_VAL, false, false, false, false};
    bool first = true;
    bool decoded = false;
    while (true) {
        if (first) {
            fetch(state, fetchedInstr);
            first = false;
        } else if (!decoded) {
            decode(fetchedInstr[0], fetchedInstr[1], fetchedInstr[2], fetchedInstr[3], &previouslyDecodedInstruction);
            fetch(state, fetchedInstr);
            decoded = true;
        } else if (previouslyDecodedInstruction.bigEndianInstr) {
            execute(&previouslyDecodedInstruction, state);
            decode(fetchedInstr[0], fetchedInstr[1], fetchedInstr[2], fetchedInstr[3], &previouslyDecodedInstruction);
            fetch(state, fetchedInstr);
        } else {
            break;
        }
        state->regs[PROGRAM_COUNTER_LOCATION] += 4; // PROGRAM COUNTER
    }
    outputState(state, noOfInstructions);
}

// Checks if the input file is null.
bool isFileNull(FILE *binaryFile) {
    if (binaryFile == NULL) {
        printf("Please use a valid file.\n");
        fclose(binaryFile);
        return EXIT_FAILURE;
    }
    return 0;
}

// Reads from the binary file.
int readFromBinFile(ARMState *state, FILE *binaryFile) {
    int arrayIndex = -1;
    while (state->memory != NULL && (fread(&state->memory[++arrayIndex], sizeof(uint8_t), 1, binaryFile) == 1)) {}
    fclose(binaryFile);
    if (state->memory == NULL)
        return ALLOCATION_ERROR;
    return arrayIndex;
}

// Where all the magic happens.
int main(int argc, char **argv) {
    if (argc != NUMBER_OF_ARGUMENTS)
        return EXIT_FAILURE;
    ARMState state;
    FILE *binaryFile;
    binaryFile = fopen(argv[1], "rb");
    if (isFileNull(binaryFile))
        return EXIT_FAILURE;
    initialiseState(&state);
    int noOfInstructions = readFromBinFile(&state, binaryFile);
    if (noOfInstructions == ALLOCATION_ERROR)
        return EXIT_FAILURE;
    //    for (int i = 0; i < noOfInstructions; i++) {
    //    printf("%02x ", state.memory[i]);
    //  }
    printf("\n"); // --> DEBUG READING FROM FILE
    pipe(&state, noOfInstructions);
    free(state.memory);
    return EXIT_SUCCESS;
}
