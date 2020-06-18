/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * This file contains all the constants required by the eecute instruction.
*/

#ifndef EXECUTE_CONSTANTS_h_
#define EXECUTE_CONSTANTS_h_

#define U_MASK 0x00800000

#define MULTIPLY_RS_MASK 0x00000F00

#define MULTIPLY_RM_MASK 0x0000000F
#define OFFSET_MASK 0x00FFFFFF
#define OFFSET_SHIFT 2
#define SIGN_BIT_MASK 0x02000000
#define SIGN_EXTEND_MASK 0xFC000000
#define Z_AND_N_FLAG_MASK 0x3FFFFFFF
#define BITS_IN_BYTE 8
#define N_FLAG_MASK 0x80000000
#define OPCODE_MASK 0x0000000F
#define S_OR_L_MASK 0x00100000
#define OFFSET_MASK_2 0x00000FFF

// enum for possible opcodes in data process instructions:
typedef enum OPCODE
{
	AND = 0, EOR = 1, SUB = 2, RSB = 3, ADD = 4, TST = 8, TEQ = 9, CMP = 10, ORR = 12, MOV = 13
} OPCODE;

#endif // EXECUTE_CONSTANTS_h_