#include "assemble.h"
#include "processHelpers.h"

/*
 * convertBranchToBinary
 * PRE - instruction is a branch instruction
 * Params - instruction / array of strings
 * Returns - the 32 bit integer equivalent of the branch instruction
 */
u_int32_t convertBranchToBinary(char **instructions, SymbTable table, int address)
{
    u_int32_t code;
    char *type = instructions[0];
    if (!strcmp(type, "beq"))
    {
        code = 0;
    }
    else if (!strcmp(type, "bne"))
    {
        code = 1;
    }
    else if (!strcmp(type, "bge"))
    {
        code = 10;
    }
    else if (!strcmp(type, "blt"))
    {
        code = 11;
    }
    else if (!strcmp(type, "bgt"))
    {
        code = 12;
    }
    else if (!strcmp(type, "ble"))
    {
        code = 13;
    }
    else if (!strcmp(type, "b") || !strcmp(type, "bal"))
    {
        code = 14;
    }
    else
    {
        perror("\nIssue setting branch code\n");
        exit(EXIT_FAILURE);
    }

    int labelAddress = returnAddressFromSymbolTable(instructions[1], table);
    int offset;
    if (labelAddress == -1)
    {
        offset = getInt(instructions[1]) - address - 8;
    }
    else
    {
        offset = labelAddress - address - 8;

        int labelNum = 0;
        if (offset < 0)
        {
            labelNum += table.numberOfItems;
        }
        offset &= 0x3FFFFFF;
        offset >>= 2;
        offset &= 0xFFFFFF;
        offset += labelNum;
    }
    u_int32_t fillerBits = 0x0a000000;
    code = code << 28;
    return code + fillerBits + (u_int32_t)offset;
}