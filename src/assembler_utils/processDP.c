#include "assemble.h"
#include "processHelpers.h"

u_int32_t getOpCode(char *opCodeString)
{
    u_int32_t opCode = 0;
    if (!(strcmp(opCodeString, "and")))
    {
        opCode = 0;
    }
    else if (!(strcmp(opCodeString, "eor")))
    {
        opCode = 1;
    }
    else if (!(strcmp(opCodeString, "sub")))
    {
        opCode = 2;
    }
    else if (!(strcmp(opCodeString, "rsb")))
    {
        opCode = 3;
    }
    else if (!(strcmp(opCodeString, "add")))
    {
        opCode = 4;
    }
    else if (!(strcmp(opCodeString, "tst")))
    {
        opCode = 8;
    }
    else if (!(strcmp(opCodeString, "teq")))
    {
        opCode = 9;
    }
    else if (!(strcmp(opCodeString, "cmp")))
    {
        opCode = 10;
    }
    else if (!(strcmp(opCodeString, "orr")))
    {
        opCode = 12;
    }
    else if (!(strcmp(opCodeString, "mov")))
    {
        opCode = 13;
    }
    return opCode;
}

/*
 * convertDPToBinary
 * Params - convertBranchToBinary
 * Returns - the 32 bit integer equivalent of the branch instruction
 */
u_int32_t convertDPToBinary(char **instruction)
{
    u_int32_t opCode = 0;
    u_int32_t setCond = 0;
    char *opCodeString = instruction[0];
    u_int32_t Rd = getInt(instruction[1]);
    Rd = Rd << 12;
    opCode = getOpCode(opCodeString);

    if (opCode == 8 || opCode == 9 || opCode == 10)
    {
        setCond = 1 << 20;
    }

    if (opCode == 0 || opCode == 1 || opCode == 2 || opCode == 3 || opCode == 4 || opCode == 12)
    {
        // add, eor, sub, rsb, add, orr
        u_int32_t Rn = (getInt(instruction[2])) << 16;
        u_int32_t immBit = 0;
        u_int32_t operand2 = processOperand2(&instruction[3]);
        if (instruction[3][0] == '#')
        {
            immBit = 1 << 25;
        }
        return TRUECOND + immBit + (opCode << 21) + setCond + Rn + Rd + operand2;
    }
    else if (opCode == 13)
    {
        //mov
        u_int32_t immBit = 0;
        u_int32_t operand2 = processOperand2(&instruction[2]);
        if (instruction[2][0] == '#')
        {
            immBit = 1 << 25;
        }
        return TRUECOND + immBit + (opCode << 21) + setCond + Rd + operand2;
    }
    else
    {
        //tst, teq, cmp
        setCond = 1 << 20;
        u_int32_t immBit = 0;
        u_int32_t operand2 = processOperand2(&instruction[2]);
        Rd <<= 4;

        if (instruction[2][0] == '#')
        {
            immBit = 1 << 25;
        }
        return TRUECOND + immBit + (opCode << 21) + setCond + Rd + operand2;
    }
    return EXIT_FAILURE;
}