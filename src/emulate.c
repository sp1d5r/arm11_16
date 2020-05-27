#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// enum for the instructions available.
typedef enum INSTRUCTION {
    MULTIPLY, BRANCH, PROCESS, TRANSFER
} INSTRUCTION;

// Struct storing the current state of the machine.
typedef struct ARMState {
    uint8_t *memory;
    uint32_t regs[17];
} ARMState;

// Struct to store the value of the 4 bit instruction in big endian, the instruction type,
// the registers which are required and the values of any other bits such as the Immediate operand.
typedef struct RETURN {
    uint32_t eConverted;
    INSTRUCTION instruction;
    uint8_t destReg, opReg1, opReg2, opReg3, bitSet1, bitSet2, upBit, loadSToreBit;
} RETURN;

// Initialises all registers and memory addressed to zero.
void initialiseState(ARMState *state) {
    for (int i = 0; i < 17; i++) {
        (*state).regs[i] = 0;
    }
    (*state).memory = (uint8_t *) calloc(65536, sizeof(uint8_t));
}

// Returns the type of instruction given in.
INSTRUCTION workOutType(uint32_t eConverted) {
    if (eConverted & 0x08000000) {
        return BRANCH;
    } else if (eConverted & 0x04000000) {
        return TRANSFER;
    } else {
        if ((eConverted & 0x000000F0) >> 4 == 9) {
            return MULTIPLY;
        } else {
            return PROCESS;
        }
    }
}

// Converts instruction to big endian.
uint32_t bigEndianConverter(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth) {
    return (uint32_t) ((fourth << 24) + (third << 16) + (second << 8) + first);
}

// Saves the register numbers and any sign/condition flags in output.
RETURN *returnRegisters(uint32_t eConverted, INSTRUCTION type, RETURN *returnThis) {
    switch (type) {
        case MULTIPLY:
            returnThis->destReg = (eConverted & 0x000F0000) >> 16; // Rd
            returnThis->opReg1 = (eConverted & 0x0000F000) >> 12; // Rn
            returnThis->opReg2 = (eConverted & 0x00000F00) >> 8; // Rs
            returnThis->opReg3 = (eConverted & 0x0000000F); // Rm
            if (eConverted & 0x00200000)
                returnThis->bitSet1 = 1; // Accumulate
            if (eConverted & 0x00100000)
                returnThis->bitSet2 = 1; // Set condition codes
            break;
        case TRANSFER:
            returnThis->destReg = (eConverted & 0x0000F000) >> 12; // Rd
            returnThis->opReg1 = (eConverted & 0x000F0000) >> 16; // Rn
            if (eConverted & 0x02000000)
                returnThis->bitSet1 = 1; // operand 2 is an immediate constant
            if (eConverted & 0x01000000)
                returnThis->bitSet2 = 1; // Pre/Post indexing bit is set
            if (eConverted & 0x00800000)
                returnThis->upBit = 1; // Up bit is Set
            if (eConverted & 0x00100000)
                returnThis->loadSToreBit = 1; // Load/Store bit is set
            break;
        case PROCESS:
            returnThis->destReg = (eConverted & 0x0000F000) >> 12; // Rd
            returnThis->opReg1 = (eConverted & 0x000F0000) >> 16; // Rn
            if (eConverted & 0x02000000)
                returnThis->bitSet1 = 1; // operand 2 is an immediate constant
            break;
        default: // DEFAULT -> BRANCH
            break;
    }
    return returnThis;
}

// Emulator decoder used to decode instructions.
RETURN *decode(uint8_t first, uint8_t second, uint8_t third, uint8_t fourth, RETURN *returnThis) {
    returnThis->eConverted = bigEndianConverter(first, second, third, fourth);
    returnThis->instruction = workOutType(returnThis->eConverted);
    returnRegisters(returnThis->eConverted, returnThis->instruction, returnThis);
    return returnThis;
}

// Check Condition Field
bool conditionChecker(uint32_t eConverted, ARMState state) {
    switch (eConverted & 0xF0000000) {

    }
}


// Multiply Instruction
void multiply(RETURN *executable, ARMState state) {
    if (conditionChecker(executable->eConverted, state)) {}
}

// Branch Instruction
void branch(RETURN *executable, ARMState state) {}

// Process Instruction
void process(RETURN *executable, ARMState state) {}

// Transfer Instruction
void transfer(RETURN *executable, ARMState state) {}

// Emulator executor used to execute instructions.
void execute(RETURN *executable, ARMState state) {
    switch (executable->instruction) {
        case MULTIPLY:
            multiply(executable, state);
            break;
        case BRANCH:
            branch(executable, state);
            break;
        case PROCESS:
            process(executable, state);
            break;
        case TRANSFER:
            transfer(executable, state);
            break;
    }
}

// An instruction is 4 bytes. The pipeline takes an instruction and decodes it
// while simultaneously executing the previously decoded instruction.
// The PC is always 8 bytes greater than the address of the instruction being executed.
void pipe(ARMState state, int noOfInstructions) {
    RETURN previouslyDecodedInstruction = {0, (INSTRUCTION) NULL, 0,
                                           0, 0, 0, 0, 0,
                                           0, 0};
    for (int i = 0; i < noOfInstructions; i++) {
        if (i > 0)
            execute(&previouslyDecodedInstruction, state);
        decode(state.memory[state.regs[15]], state.memory[state.regs[15] + 1],
               state.memory[state.regs[15] + 2],
               state.memory[state.regs[15] + 3], &previouslyDecodedInstruction);
        state.regs[15] += 4;
    }
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
int readFromBinFile(ARMState state, FILE *binaryFile) {
    int arrayIndex = -1;
    while (state.memory != NULL && (fread(&state.memory[++arrayIndex], sizeof(uint8_t), 1, binaryFile) == 1)) {}
    fclose(binaryFile);
    if (state.memory == NULL)
        return -1;
    return arrayIndex;
}

// Where all the magic happens.
int main(int argc, char **argv) {
    if (argc != 2)
        return EXIT_FAILURE;
    ARMState state;
    FILE *binaryFile;
    binaryFile = fopen(argv[1], "rb");
    if (isFileNull(binaryFile))
        return EXIT_FAILURE;
    initialiseState(&state);
    int n = readFromBinFile(state, binaryFile);
    if (n == -1)
        return EXIT_FAILURE;
/*    for (int i = 0; i < n; i++) {
        printf("%02x ", state.memory[i]);
    }*/
    pipe(state, n / 4);
    free(state.memory);
    return EXIT_SUCCESS;
}
