#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

// Removed -Werror from Makefile but make sure to reinclude it later.

#include "defines.c"

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
typedef struct DECODE {
    uint32_t bigEndianInstr;
    INSTRUCTION instruction;
    uint8_t destReg, opReg1, opReg2, opReg3;
    bool op1, op2, upBit, loadSToreBit;
} DECODE;

typedef enum SHIFTTYPE {
    LOGICALLEFT, LOGICALRIGHT, ARITHMETIC, ROTATERIGHT
} SHIFTTYPE;

typedef struct SHIFTER {
    bool op2IsImm;
    uint32_t valueForImm;
    uint32_t rotateAmount;
    uint32_t registerToGetValueFrom;
    uint32_t registerToGetValueFrom2;
    uint32_t integerForRegister;
    SHIFTTYPE shiftType;
    bool isShiftReg;
    bool setFlags;
} SHIFTER;

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
        if ((((bigEndianInstr & MULTIPLY_INSTR_1_MASK) >> BITS_IN_NIBBLE) == 0x9) &&
            !(bigEndianInstr & MULTIPLY_INSTR_2_MASK)) { // MAYBE ONE OF THE ANDS CAN BE REMOVED? CHECK WHILE TESTING.
            return MULTIPLY;
        } else {
            return PROCESS;
        }
    }
}

// Converts instruction to big endian.
uint32_t bigEndianConverter(uint32_t byte1, uint32_t byte2, uint32_t byte3, uint32_t byte4) {
    return (byte4 << BITS_IN_THREE_BYTES) + (byte3 << BITS_IN_TWO_BYTES) + (byte2 << BITS_IN_BYTE) + byte1;
}

// Saves the register numbers and any sign/condition flags in output.
void decodeHelper(uint32_t bigEndianInstr, INSTRUCTION instrType, DECODE *instrToDecode) {
    switch (instrType) {
        case MULTIPLY:
            instrToDecode->destReg = (bigEndianInstr & MULTIPLY_RD_MASK) >> BITS_IN_TWO_BYTES; // Rd
            instrToDecode->opReg1 = (bigEndianInstr & MULTIPLY_RN_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE); // Rn
            instrToDecode->opReg2 = (bigEndianInstr & MULTIPLY_RS_MASK) >> BITS_IN_BYTE; // Rs
            instrToDecode->opReg3 = (bigEndianInstr & MULTIPLY_RM_MASK); // Rm
            instrToDecode->op1 = bigEndianInstr & ACCUMULATE_MASK; // Accumulate Bit Set?
            instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
            break;
        case TRANSFER:
            instrToDecode->destReg = (bigEndianInstr & DATA_RD_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE); // Rd
            instrToDecode->opReg1 = (bigEndianInstr & DATA_RN_MASK) >> BITS_IN_TWO_BYTES; // Rn
            instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Operand 2 type? Immediate if 1 else register
            instrToDecode->op2 = bigEndianInstr & P_MASK; // Pre/Post indexing bit is set?
            instrToDecode->upBit = bigEndianInstr & U_MASK; // Up bit is set?
            instrToDecode->loadSToreBit = bigEndianInstr & S_OR_L_MASK; // Load/Store bit is set?
            break;
        case PROCESS:
            instrToDecode->destReg = (bigEndianInstr & DATA_RD_MASK) >> (BITS_IN_BYTE + BITS_IN_NIBBLE); // Rd
            instrToDecode->opReg1 = (bigEndianInstr & DATA_RN_MASK) >> BITS_IN_TWO_BYTES; // Rn
            instrToDecode->op1 = bigEndianInstr & IMMEDIATE_MASK; // Operand 2 type? Immediate if 1 else register
            instrToDecode->op2 = bigEndianInstr & S_OR_L_MASK; // Set condition codes?
            break;
        default:
            break;
    }
}

// Emulator decoder used to decode instructions.
void decode(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4, DECODE *instrToDecode) {
    instrToDecode->bigEndianInstr = bigEndianConverter(byte1, byte2, byte3, byte4);
    instrToDecode->instruction = workOutType(instrToDecode->bigEndianInstr);
    decodeHelper(instrToDecode->bigEndianInstr, instrToDecode->instruction, instrToDecode);
}

// Check Condition Field
bool conditionChecker(uint32_t bigEndianInstr, ARMState *state) {
    return (bigEndianInstr & COND_FIELD_MASK) == state->regs[CPSR_LOCATION]; // NO NEED TO SHIFT HERE!
}


uint32_t rotateRight(uint32_t operand, uint8_t n) {
    uint32_t a = operand >> n;
    uint32_t b = operand << (32 - n);
    return a | b;
}

//Returns shift type
SHIFTER *shiftType(uint32_t eConverted, SHIFTER *blank) {
    uint32_t typemask = 0x60;
    uint32_t distanceMask = 0xf00;
    int opCode = (eConverted & 0x60) >> 5;
    SHIFTTYPE type;
    type = (eConverted & typemask) >> 5; // Is this required?
    blank->rotateAmount = ((eConverted & distanceMask) >> 8) * 2;
    blank->op2IsImm = ((0x02000000 & eConverted) >> 25);
    uint32_t valueForImmMask = 0xff;
    blank->valueForImm = valueForImmMask & eConverted;
    blank->registerToGetValueFrom = 0xf & eConverted;
    blank->integerForRegister = ((0xf80 & eConverted) >> 7);
    blank->setFlags = (eConverted & 0x10000) >> 20;
    blank->isShiftReg = (eConverted >> 4) & 0x1;
    blank->registerToGetValueFrom2 = (eConverted >> 8) & 0xf;

    if (opCode == 3) {
        type = ROTATERIGHT;
    } else if (opCode == 2) {
        type = ARITHMETIC;
    } else if (opCode == 1) {
        type = LOGICALRIGHT;
    } else {
        type = LOGICALLEFT;
    }
    blank->shiftType = type;

    return blank;
}

uint32_t shift(SHIFTER shifter, ARMState *state) {
    uint32_t result;
    int carryout;
    if (!shifter.op2IsImm) {
        int valueToBeShifted = state->regs[shifter.registerToGetValueFrom];
        if (shifter.isShiftReg){
            uint32_t valueRs = state->regs[shifter.registerToGetValueFrom2];
            shifter.integerForRegister = valueRs & 0xf;
        }

        switch (shifter.shiftType) {
            case LOGICALLEFT:
                carryout = (valueToBeShifted & (1 << (31 - shifter.integerForRegister)))
                        >> (31 - shifter.integerForRegister);
                result = valueToBeShifted << shifter.integerForRegister;
                break;
            case LOGICALRIGHT:
                carryout = (valueToBeShifted & (1 << (shifter.integerForRegister - 1)))
                        >> (shifter.integerForRegister - 1);
                result = valueToBeShifted >> shifter.integerForRegister;
                break;
            case ARITHMETIC:
                carryout = (valueToBeShifted & (1 << (shifter.integerForRegister - 1)))
                        >> (shifter.integerForRegister - 1);
                result = valueToBeShifted >> shifter.integerForRegister;
                if ((valueToBeShifted & 1 << 31) == 1 << 31) { //if the last bit is one then the number is negative
                    int mask = 1 << 31;
                    for (int i = 0; i < valueToBeShifted; i++) {
                        result = result | mask;
                        mask = mask >> 1;
                    }
                }
                break;
            case ROTATERIGHT:
                carryout = (valueToBeShifted & (1 << (shifter.integerForRegister - 1)))
                        >> (shifter.integerForRegister - 1);
                uint32_t rightSide = valueToBeShifted >> shifter.valueForImm;
                uint32_t leftSide = valueToBeShifted << (32 - shifter.valueForImm);
                result = leftSide || rightSide;
                break;
        }
    } else {
        result = rotateRight(shifter.valueForImm, shifter.rotateAmount);
        int carryoutMask = 1 << (shifter.rotateAmount - 1);
        carryout = (shifter.valueForImm & carryoutMask) >> (shifter.rotateAmount - 1);
    }
    int ZFLag = (result == 0) ? 1 : 0;
    int NFlag = ((result & 0x800000000) == 0x800000000) ? 1 : 0;
    int CSPRSet = NFlag >> 31 | ZFLag >> 30 | carryout >> 29;
    state->regs[CPSR_LOCATION] = shifter.setFlags ? (state->regs[CPSR_LOCATION] & 0x1fffffff) | CSPRSet
                                                  : state->regs[CPSR_LOCATION];

    return result;
}

// Branch Instruction
void branch(DECODE *instrToDecode, ARMState *state) {
    uint32_t shiftedOffset = (instrToDecode->bigEndianInstr & OFFSET_MASK) << OFFSET_SHIFT;
    bool signBit = (shiftedOffset & SIGN_BIT_MASK);
    if (signBit)
        state->regs[PROGRAM_COUNTER_LOCATION] += (shiftedOffset | SIGN_EXTEND_MASK) - PC_OFFSET;
    else
        state->regs[PROGRAM_COUNTER_LOCATION] += shiftedOffset - PC_OFFSET;
}

// Multiply Instruction
void multiply(DECODE *instrToDecode, ARMState *state) {
    uint32_t rm = instrToDecode->opReg3, rs = instrToDecode->opReg2, rn = instrToDecode->opReg1;
    bool accumulate = instrToDecode->op1;
    uint32_t result = rm * rs + (rn * accumulate);
    state->regs[instrToDecode->destReg] = result;
    if (instrToDecode->op2) {
        uint32_t nBit = result & N_MASK;
        uint32_t zBit = !((bool) result) << 30; // Left shifting by 30 adjusts the zero flag accordingly.
        state->regs[CPSR_LOCATION] = nBit + zBit + (state->regs[CPSR_LOCATION] & Z_AND_N_MASK);
    }
}

// PROCESS
bool getImmediate(DECODE *instrToDecode) {
    return instrToDecode->op1;
}

bool getSetCond(DECODE *instrToDecode) {
    return instrToDecode->op2;
}

uint8_t getRnNumber(DECODE *instrToDecode) {
    return instrToDecode->opReg1;
}

uint8_t getRdNumber(DECODE *instrToDecode) {
    return instrToDecode->destReg;
}

uint8_t getOpCode(uint32_t bigEndianInstr) {
    uint32_t opCode = bigEndianInstr >> 21; // Right shifting by 21 puts opCode in the bottom 4 bytes.
    uint32_t mask = 0x0000000F;
    return (uint8_t) opCode & mask;
}

uint16_t getOperand2(uint32_t bigEndianInstr) {
    uint32_t mask = 0x00000FFF;
    return (uint16_t) bigEndianInstr & mask;
}

uint8_t getCond(uint32_t bigEndianInstr) {
    bigEndianInstr >>= BITS_IN_THREE_BYTES + BITS_IN_NIBBLE;
    uint32_t mask = 0x0000000F;
    return (uint8_t) bigEndianInstr & mask;
}


// Process Instruction
uint32_t processOperand2(uint32_t bigEndianInstr, ARMState *state) {
    SHIFTER blank;
    shiftType(bigEndianInstr, &blank);
    uint32_t result = shift(blank, state);
    return result;
}

uint32_t processOpCode(DECODE *decoded, ARMState *state) {
    uint8_t opCode = getOpCode(decoded->bigEndianInstr);
    uint32_t rNContent = state->regs[getRnNumber(decoded)];
    uint32_t processedOperand2 = processOperand2(decoded->bigEndianInstr, state);
    uint32_t result;
    switch (opCode) {
        case 0:
            result = rNContent & processedOperand2;
            break;
        case 1:
            result = rNContent ^ processedOperand2;
            break;
        case 2:
            result = rNContent - processedOperand2;
            break;
        case 3:
            result = processedOperand2 - rNContent;
            break;
        case 4:
            result = processedOperand2 + rNContent;
            break;
        case 8:
            result = rNContent & processedOperand2;
            break;
        case 9:
            result = rNContent ^ processedOperand2;
            break;
        case 10:
            result = rNContent - processedOperand2;
            break;
        case 12:
            result = rNContent | processedOperand2;
            break;
        case 13:
            result = processedOperand2;
            break;
        default:
            break;
    }
    return result;
}

void process(DECODE *executableInstr, ARMState *state) {}




// Transfer Instruction
void transfer(DECODE *executableInstr, ARMState *state) {}

// Emulator executor used to execute instructions.
void execute(DECODE *executableInstr, ARMState *state) {
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
                printf("INVALID INSTRUCTION ENTERED!!\n");
                break;
        }
    }
}

// Outputs the state of all Non Zero Memory after program has ended.
void outputNonZeroMem(ARMState *state, int noOfBytesInFile) {
    printf("Non-Zero memory:");
    for (int i = 0; i < noOfBytesInFile; i += NUMBER_OF_BYTES_IN_INSTR) {
        if (state->memory[i] || state->memory[i + 1] || state->memory[i + 2] || state->memory[i + 3]) {
            printf("\n0x%08x: 0x%02x%02x%02x%02x", i, state->memory[i], state->memory[i + 1], state->memory[i + 2],
                   state->memory[i + 3]);
        }
    }
}

// Outputs the state of registers after program has ended.
void outputState(ARMState *state, int noOfBytesInFile) {
    uint32_t regState, i;
    for (i = 0; i < NUMBER_OF_REGISTERS - 2; ++i) {
        regState = state->regs[i];
        printf("$%-3d:%11d (0x%08x)\n", i, regState, regState);
    }
    printf("PC  :%11d (0x%08x)\n", state->regs[PROGRAM_COUNTER_LOCATION], state->regs[PROGRAM_COUNTER_LOCATION]);
    printf("CPSR:%11d (0x%08x)\n", state->regs[CPSR_LOCATION], state->regs[CPSR_LOCATION]);
    outputNonZeroMem(state, noOfBytesInFile);
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
void pipe(ARMState *state, int noOfBytesInFile) {
    uint32_t fetchedInstr[NUMBER_OF_BYTES_IN_INSTR];
    DECODE previouslyDecodedInstruction = {INIT_ZERO_VAL, NONE, INIT_ZERO_VAL, INIT_ZERO_VAL, INIT_ZERO_VAL,
                                           INIT_ZERO_VAL, false, false, false, false};
    bool firstIteration = true;
    bool firstDecode;
    while (true) {
        if (firstIteration) {
            fetch(state, fetchedInstr);
            firstIteration = false;
            firstDecode = true;
        } else if (firstDecode) {
            decode(fetchedInstr[0], fetchedInstr[1], fetchedInstr[2], fetchedInstr[3], &previouslyDecodedInstruction);
            fetch(state, fetchedInstr);
            firstDecode = false;
        } else if (previouslyDecodedInstruction.bigEndianInstr) {
            execute(&previouslyDecodedInstruction, state);
            decode(fetchedInstr[0], fetchedInstr[1], fetchedInstr[2], fetchedInstr[3], &previouslyDecodedInstruction);
            fetch(state, fetchedInstr);
        } else {
            break;
        }
        state->regs[PROGRAM_COUNTER_LOCATION] += NUMBER_OF_BYTES_IN_INSTR; // PROGRAM COUNTER
    }
    outputState(state, noOfBytesInFile);
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
    binaryFile = fopen(argv[ARG_WITH_FILE], "rb");
    if (isFileNull(binaryFile))
        return EXIT_FAILURE;
    initialiseState(&state);
    int noOfBytesInFile = readFromBinFile(&state, binaryFile);
    if (noOfBytesInFile == ALLOCATION_ERROR)
        return EXIT_FAILURE;
    pipe(&state, noOfBytesInFile);
    free(state.memory);
    return EXIT_SUCCESS;
}
