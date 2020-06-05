#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "assembler_utils/assemble.h"

#define MAX_LINE_LENGTH 511

/*
 * what the goal of the program is supposed to be:
 *  - takes in two arguments 1) arm assembly instruction, 2) binary filename
 *  - it separates out the file into instructions each instruction is a line (separate by \n)
 *  - it passes through these instructions and looks for labels, a lable is a string followed by a colon i.e. "label:"
 *  - it creates a symbol table, this is a label and the appropriate address that follows it
 *  - looks like this: labels are also symbols
 *  ____________________________
 *  | lables    | addressses    |
 *  |label1     | 0             |
 *  |label2     | 4             |
 *  |label3     | 8             |
 *  |label4     | 12            |
 * -------------------------------
 *  - that's supposed to be useful because in the second pass it replaces the label with the address that is linked to it
 *  - in the second pass, it takes the instruction and evaluates it, if it's a label then it get's replaced by the
 *      address associated with the label
 *  - otherwise it get's converted to the binary equivalent and then gets written to the binary file.
 *
 * How this Program Works:
 * 1) tests the number of arguments lines 703ish-709ish
 * 2) creates an array of strings (each string being an instruction (each instruction being a line))
 * 3) creates a symbol table (check below....)
 * 4) populate the symbol table using the array of strings
 * 5) in the assemble function, you take the array of strings, convert them into the appropriate form
 *      (using helper function)
 *      these helper functions, convert the assembler code into the appropriate binary form
 *      then the function takes in the value and writes it to the next.
 *
 * what we are doing now:
 * - quite a few bugs i.e. seg faults
 * - the symbol tabel isn't working // the first pass is being quite shit it refuses to add in the strings
 * - we've discussed changing the form completely holding the instructions into a new data structure ^ if it doesnt work
 * - also there's still a few functions which need to be made i.e. ldr and sdt
 * - ^^ shifts and shit
 */

/*
 * createTable
 * returns an initialised Symbol Address table
 */
SymbTable createTable()
{
  SymbTable table;
  table.numberOfItems = 0;
  table.labels = (char **)calloc(1, sizeof(char *));
  table.memoryAddresses = (int *)calloc(1, sizeof(int));
  return table;
}

int registerRep(char *string)
{
  for (int i = 0; i < strlen(string); i++)
  {
    if (string[i] == 'r')
    {
      return 1;
    }
  }
  return 0;
}

/*
 * createStringArray
 * Params - array of strings, length of the array, the max size of each string
 */
void createStringArray(char **array, int length, int maxSize)
{
  for (int i = 0; i < length; i++)
  {
    array[i] = (char *)calloc(maxSize, sizeof(char));
  }
}

/*
 * fileLength
 * Params - takes in the filename string
 * Returns - the number of lines in the file (number of instructions in file)
 */
int fileLength(char *filename)
{
  int lines = 0;
  FILE *file;
  file = fopen(filename, "r");
  char c = fgetc(file);
  while (c != EOF)
  {
    if (c == '\n')
    {
      lines += 1;
    }
    c = fgetc(file);
  }
  fclose(file);
  return lines;
}

/*
 * longestLine
 * Params - takes in the filename string
 * Returns - calculates the largest number of characters on a line
 */
int longestLine(char *filename)
{
  int max = 1;
  FILE *file;
  file = fopen(filename, "r");
  char c = fgetc(file);
  int runningTotal = 1;
  while (c != EOF)
  {
    if (c != '\n')
    {
      runningTotal += 1;
    }
    else
    {
      max = (max < runningTotal) ? runningTotal : max;
      runningTotal = 1;
    }
    c = fgetc(file);
  }
  fclose(file);

  return max;
}

/*
 * arrayOfArmInstructions
 * Params - the filename string, the number of lines in file
 * Returns - the array of arm instructiosn saved in the file
 */
char **arrayOfArmInstructions(char *filename, int lines)
{
  // create a 2d array of instructions
  printf("%i", lines);
  FILE *file;
  file = fopen(filename, "r");
  int longestLine = fileLength(filename);
  char **text = (char **)calloc(lines, sizeof(char *));
  createStringArray(text, lines + 1, longestLine + 20);
  char c = fgetc(file);
  int i = 0;
  int j = 0;
  while (c != EOF)
  {
    if (c != '\n')
    {
      text[i][j] = c;
      j++;
    }
    else
    {
      text[i][j] = ' ';
      text[i][j + 1] = 'e';
      text[i][j + 2] = 'n';
      text[i][j + 3] = 'd';
      text[i][j + 4] = '\0';
      i++;
      j = 0;
    }
    c = fgetc(file);
  }
  text[lines] = "FIN";
  for (i = 0; i <= lines; i++)
  {
    printf("%s", text[i]);
    printf("\n");
  }

  return text;
}

/*
 * checkForColon
 * Params - a word
 * Returns - 1 if there is a colon at end or 0 if there is not
 */
int checkForColon(char *word)
{
  int i = 0;
  while (word[i] != '\0')
  {
    if (word[i] == ':')
    {
      return 1;
    }
    i++;
  }
  return 0;
}

/*
 * removeLastChar
 * Params - string word, string buffer
 * Returns - nothing
 * (changes the value of buffer to the string without the last char)
 */
void removeLastChar(char *word, char *buffer)
{
  int i = 0;
  while (word[i + 1] != '\0')
  {
    buffer[i] = word[i];
    i++;
  }
}

/*
 * addToSymbTable
 * Params - the label string, the address int, the current SymbTable
 * Returns - nothing
 * (updates the value of the symbtable to include the now row)
 */
void addToSymbTable(char *label, int address, SymbTable *array)
{
  int currentTotal = array->numberOfItems;
  array->labels = (char **)realloc(array->labels, (currentTotal + 2) * sizeof(char *));
  array->memoryAddresses = (int *)realloc(array->labels, (currentTotal + 2) * sizeof(int));
  array->labels[currentTotal] = (char *)calloc(strlen(label), sizeof(char));
  for (int i = 0; i < strlen(label); i++)
  {
    array->labels[currentTotal][i] = label[i];
  }
  array->memoryAddresses[currentTotal] = address;
  array->numberOfItems = currentTotal + 1;
}

/*
 * returnAddressFromSymbolTable
 * Params - label string, the current SymbTable
 * Returns - if the label is in table then return the address associated otherwise return -1
 */
int returnAddressFromSymbolTable(char *stringTarget, SymbTable lookUp)
{
  if (lookUp.numberOfItems == 0)
  {
    return -1;
  }
  for (int i = 0; i < lookUp.numberOfItems; i++)
  {
    if (!(strcmp(lookUp.labels[i], stringTarget)))
    {
      return lookUp.memoryAddresses[i];
    }
  }
  return -1;
}

/*
 * operandTotal
 * Params - array of strings
 * Returns - the number of strings in the array (number of operands)
 */
int operandTotal(char **array)
{
  int i = 0;
  while (strcmp(array[i], "end"))
  {
    i++;
  }
  return i;
}

/*
 * pow1
 * Params - int a, int b
 * Returns - a^b
 */
int pow1(int a, int b)
{
  int result = 1;
  while (b != 0)
  {
    result *= a;
    b--;
  }
  return result;
}

/*
 * getStringSize
 * Params - a string values
 * Returns - the number of strings values can be split up into according to the delimiter " ,:" and " ,.-"
 */
int getStringSize(char *values)
{
  char cloned_values[511];
  int i = 0;
  while (values[i] != '\0')
  {
    cloned_values[i] = values[i];
    i++;
  }
  cloned_values[i] = '\0';
  char *separated_values;
  i = 0;
  separated_values = strtok(cloned_values, " ,:");
  while (separated_values != NULL)
  {
    separated_values = strtok(NULL, " ,.-");
    i++;
  }
  return i;
}

/*
 * splitUp
 * Params - a string
 * Returns - an array of strings after splitting it up appropriately
 */
char **splitUp(char *string)
{
  char buffer[511];
  strcpy(buffer, string);
  int size = getStringSize(string);
  char *separated_values;
  char **array = (char **)calloc(size, sizeof(char *));
  separated_values = strtok(buffer, " ,:[]");
  int i = 0;
  while (separated_values != NULL)
  {
    array[i] = separated_values;
    separated_values = strtok(NULL, " ,.-");
    i++;
  }

  return array;
}

/*
 * firstPass
 * Params - symbtable, and array of strings (instructions)
 * Returns - nothing
 * FINISHED LETS GOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO
 */
void firstPass(SymbTable *table, char **instructions)
{
  char **labels;
  int totalLabels = 0;
  int i = 0;
  while (strcmp(instructions[i], "FIN"))
  {
    if (checkForColon(instructions[i]))
    {
      totalLabels++;
    }
    i++;
  }
  i = 0;
  if (totalLabels == 0)
  {
    table->numberOfItems = 0;
    table->memoryAddresses = (int *)calloc(1, sizeof(int));
    table->labels = (char **)calloc(1, sizeof(char *));
    table->labels[0] = (char *)calloc(6, sizeof(char));
    char *empty = "empty";
    for (int k = 0; k < strlen("empty"); k++)
    {
      table->labels[0][k] = empty[k];
    }
    table->memoryAddresses[0] = 0;
  }
  else
  {
    table->numberOfItems = totalLabels;
    labels = (char **)calloc(totalLabels, sizeof(char *));
    for (int k = 0; k < totalLabels; k++)
    {
      labels[k] = (char *)calloc(15, sizeof(char));
    }
    table->memoryAddresses = (int *)calloc(totalLabels, sizeof(int));
    totalLabels = 0;
    while (strcmp(instructions[i], "FIN"))
    {
      if (checkForColon(instructions[i]))
      {
        char **split = splitUp(instructions[i]);
        for (int j = 0; j < strlen(split[0]); j++)
        {
          labels[totalLabels][j] = split[0][j];
        }
        table->memoryAddresses[totalLabels] = (i - totalLabels) * 4;
        totalLabels++;
      }
      i++;
    }
    table->labels = labels;
  }
}

/*
 * hexStringToInt
 * Params - takes a string
 * Returns - the integer equivalent of the hex equivalent
 */
int hexStringToInt(char *string)
{
  int size = strlen(string);
  int result = 0;
  int power = 0;
  for (int i = size - 1; i >= 0; i--)
  {
    if (string[i] == ']')
    {
      continue;
    }
    int curr = string[i];
    if (curr >= 97 && curr >= 102)
    {
      result += (curr - 87) * pow1(16, power);
    }
    else
    {
      result += (curr - 48) * pow1(16, power);
    }
    power++;
  }
  return result;
}

/*
 * getInt
 * Params - Takes a string values
 * Returns - the integer equivalent of the values
 */
int getInt(char *values)
{
  if (values[0] == '[' || values[0] == '=')
  {
    return getInt(values + 1);
  }
  else if (!(values[0] == '0' && values[1] == 'x'))
  {
    return atoi(values + 1);
  }
  else
  {
    return hexStringToInt(values + 2);
  }
}

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
    code = -1;
    printf("\nissue setting branch code\n");
  }
  int labelAddress = returnAddressFromSymbolTable(instructions[1], table);
  u_int32_t offset = (labelAddress == -1) ? getInt(instructions[1]) - address - 8 : labelAddress - address - 8;
  u_int32_t fillerBits = 0x0a000000;
  code = code << 28;
  return code + fillerBits + offset;
}

/*
* convertMultiplyToBinary
* PRE - the instruction is a multiply instruction
* Params - instruction / array of strings
* Returns - the 32 bit integer equivalent of the multiply instruction
*/
u_int32_t convertMultiplyToBinary(char **instructions)
{
  char *opCodeString = instructions[0];
  u_int32_t Rd = getInt(instructions[1]) << 16;
  u_int32_t Rm = getInt(instructions[2]);
  u_int32_t Rs = getInt(instructions[3]) << 8;
  u_int32_t fillerBits = 0x90;
  if (!(strcmp(opCodeString, "mla")))
  {
    u_int32_t accFlag = 1 << 21;
    u_int32_t Rn = getInt(instructions[4]);
    Rn = Rn << 12;
    return TRUECOND + accFlag + Rd + Rn + Rm + Rs + fillerBits;
  }
  else
  {
    return TRUECOND + Rd + Rm + Rs + fillerBits;
  }
}

/*
 * convertDPToBinary
 * Params - convertBranchToBinary
 * Returns - the 32 bit integer equivalent of the branch instruction
 */
u_int32_t convertDPToBinary(char **instruction)
{
  u_int32_t opCode = 0;
  u_int32_t testCode = 0;
  u_int32_t shift = 0;
  u_int32_t valueToShift = 0;
  int arguements = operandTotal(instruction);
  u_int32_t op2;
  char *opCodeString = instruction[0];
  u_int32_t Rd = getInt(instruction[1]);
  Rd = Rd << 12;

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

  if (opCode == 8 || opCode == 9 || opCode == 10)
  {
    testCode = 1 << 20;
  }
  opCode = opCode << 21;
  if ((opCode >> 21) == 13 || (opCode >> 21) == 9 || (opCode >> 21) == 8 || (opCode >> 21) == 10)
  {
    if (arguements == 3 && !testCode)
    {
      u_int32_t immFlag = 1 << 25;
      u_int32_t offset = getInt(instruction[2]);
      return TRUECOND + opCode + immFlag + testCode + Rd + offset;
    }
    else if (arguements == 3)
    {
      if (registerRep(instruction[2]))
      {
        u_int32_t Rn = getInt(instruction[2]) << 16;
        return TRUECOND + Rn + Rd + opCode + testCode;
      }
      else
      {
        u_int32_t Rm = getInt(instruction[2]);
        char *shiftType = instruction[3];
        if (!(strcmp(shiftType, "lsl")))
        {
          shift = 0;
        }
        else if (!(strcmp(shiftType, "lsr")))
        {
          shift = 1;
        }
        else if (!(strcmp(shiftType, "asr")))
        {
          shift = 2;
        }
        else if (!(strcmp(shiftType, "ror")))
        {
          shift = 3;
        }
        shift = shift << 5;
        valueToShift = getInt(instruction[4]);
        valueToShift = valueToShift << 7;
        return TRUECOND + opCode + testCode + valueToShift + Rm + Rd + shift;
      }
    }
  }
  else
  {
    u_int32_t Rn = getInt(instruction[2]) << 16;
    if (arguements == 4)
    {
      op2 = getInt(instruction[3]);
      return op2 + Rn + TRUECOND + Rd + (1 << 20) + opCode;
    }
    else
    {
      u_int32_t Rm = getInt(instruction[3]);

      char *shiftType = instruction[4];
      if (!(strcmp(shiftType, "lsl")))
      {
        shift = 0;
      }
      else if (!(strcmp(shiftType, "lsr")))
      {
        shift = 1;
      }
      else if (!(strcmp(shiftType, "asr")))
      {
        shift = 2;
      }
      else if (!(strcmp(shiftType, "ror")))
      {
        shift = 3;
      }
      shift = shift << 5;
      valueToShift = getInt(instruction[5]);
      valueToShift = valueToShift << 7;

      return valueToShift + TRUECOND + shift + Rm + opCode + testCode + Rn;
    }
  }
  return EXIT_FAILURE;
}

/*
 * convertSDTToBinary
 * PRE - instruction is a single data transfer instruction
 * Params - instruction / array of strings
 * Returns - the 32 bit integer equivalent of the single data transfer instruction
 */
/*
u_int32_t convertSDTToBinary(char **instructions){
    char *instructionName = instructions[0];
    u_int32_t Rd = getInt(instructions[1]);
    u_int32_t address = instructions[2];

    if (!(strcomp(instructionName, "ldr"))){

    } else if (!(strcomp(instructionName, "str"))){

    }
}
*/

u_int32_t convertSDTToBinary(char **instructions)
{
  char *loadStore = instructions[0];
  strcpy(loadStore, instructions[0]);
  u_int32_t fillerBits = 0x04000000;
  u_int32_t Rd = getInt(instructions[1]);
  int addressCount = operandTotal(instructions) - 2;
  u_int32_t loadFlag = 0;
  if (!(strcmp(loadStore, "ldr")))
  {
    // u_int32_t loadFlag = 1 << 20;
  }

  if (addressCount == 1)
  {
    // do case for numeric constant i.e. <=expression>
    if (instructions[2][0] == '=')
    {
      int numericExpression; //get the integer value of the string after the = sign
      numericExpression = getInt(&(instructions[2])[1]);
      if (numericExpression < 0xFF)
      {
        // create a move instruction
        char **movInstruction = (char **)calloc(3, sizeof(char *));
        createStringArray(movInstruction, 3, 10);
        movInstruction[0] = "mov";
        movInstruction[1] = instructions[1];
        char hashtag[] = "#";
        movInstruction[2] = strcat(hashtag, &(instructions[2])[1]);
        return convertDPToBinary(movInstruction);
      }
      else
      {
        // place numeric expression at end of assembler file
        // calculate offset between the current instruction and the newly generated on
        // recurse on "ldr, (instruction[1]), [PC, offset]"
        char **newLdrInstruction = (char **)calloc(4, sizeof(char *));
        createStringArray(newLdrInstruction, 4, 10);
        newLdrInstruction[0] = "ldr";
        newLdrInstruction[1] = instructions[1];
        newLdrInstruction[2] = "[PC";
        newLdrInstruction[3] = "offset]"; // <-- calculate this one...
        return convertSDTToBinary(newLdrInstruction);
      }
    }
    else
    {
      // the value stored in teh registers is [rX]
      // Rn = get int of the value held at instruction[3] set offset to 0
      u_int32_t Rn = getInt(instructions[2]) << 16;
      return TRUECOND + fillerBits + loadFlag + Rn + Rd;
    }
  }
  else if (addressCount == 2)
  {
    // check if post or pre indexing
    int pFlag = 0;
    if (strchr(instructions[2], ']') != NULL)
    {
      pFlag = 0;
    }
    else
    {
      pFlag = 1 << 24;
    }

    u_int32_t Rn = 0;
    u_int32_t offset = 0;
    if (pFlag == 0)
    {
      // post indexing
      Rn = getInt(instructions[2]) << 16;
      offset = getInt(&(instructions[3])[1]);
    }
    else
    {
      // pre indexing
      // figure out how to get rid of the brackets ..
      Rn = getInt(instructions[2]) << 16;
      offset = getInt(&(instructions[3])[1]);
    }
    return TRUECOND + fillerBits + pFlag + loadFlag + Rd + Rn + offset;
  }
  return EXIT_FAILURE;
}

/*
 * getMnemonic
 * Params - array of strings (split up arm instructions)
 * Returns - the enum MNEMONIC of the instruction
 */
MNEMONICS getMnemonic(char **instruction)
{
  char *opcode = instruction[0];
  if (!strcmp(opcode, "add"))
  {
    return ADD;
  }
  else if (!strcmp(opcode, "sub"))
  {
    return SUB;
  }
  else if (!strcmp(opcode, "rsb"))
  {
    return RSB;
  }
  else if (!strcmp(opcode, "and"))
  {
    return AND;
  }
  else if (!strcmp(opcode, "eor"))
  {
    return EOR;
  }
  else if (!strcmp(opcode, "orr"))
  {
    return ORR;
  }
  else if (!strcmp(opcode, "mov"))
  {
    return MOV;
  }
  else if (!strcmp(opcode, "tst"))
  {
    return TST;
  }
  else if (!strcmp(opcode, "teq"))
  {
    return TEQ;
  }
  else if (!strcmp(opcode, "cmp"))
  {
    return CMP;
  }
  else if (!strcmp(opcode, "mul"))
  {
    return MUL;
  }
  else if (!strcmp(opcode, "mla"))
  {
    return MLA;
  }
  else if (!strcmp(opcode, "ldr"))
  {
    return LDR;
  }
  else if (!strcmp(opcode, "str"))
  {
    return STR;
  }
  else if (!strcmp(opcode, "beq"))
  {
    return BEQ;
  }
  else if (!strcmp(opcode, "bne"))
  {
    return BNE;
  }
  else if (!strcmp(opcode, "bge"))
  {
    return BGE;
  }
  else if (!strcmp(opcode, "blt"))
  {
    return BLT;
  }
  else if (!strcmp(opcode, "bgt"))
  {
    return BGT;
  }
  else if (!strcmp(opcode, "b"))
  {
    return B;
  }
  else if (!strcmp(opcode, "ble"))
  {
    return BLE;
  }
  else if (!strcmp(opcode, "andeq"))
  {
    return ANDEQ;
  }
  else
  {
    return LABEL;
  }
}

/*
 * littleEndianConv
 * Params - big endian 32 bit instruction
 * Returns - little endian 32 bit instruction
 */
u_int32_t littleEndianConv(u_int32_t instruction)
{
  u_int32_t firstByte = (instruction & 0xff000000) >> 24;
  u_int32_t secondByte = (instruction & 0x00ff0000) >> 8;
  u_int32_t thirdByte = (instruction & 0x0000ff00) << 8;
  u_int32_t fourthByte = (instruction & 0x000000ff) << 24;
  return firstByte | secondByte | thirdByte | fourthByte;
}

void writeToFile(FILE *file, u_int32_t instruction)
{
  u_int32_t firstByte = (instruction & 0xff000000) >> 24;
  u_int32_t secondByte = (instruction & 0x00ff0000) >> 16;
  u_int32_t thirdByte = (instruction & 0x0000ff00) >> 8;
  u_int32_t fourthByte = (instruction & 0x000000ff);
  fwrite(&firstByte, sizeof(u_int8_t), 1, file);
  fwrite(&secondByte, sizeof(u_int8_t), 1, file);
  fwrite(&thirdByte, sizeof(u_int8_t), 1, file);
  fwrite(&fourthByte, sizeof(u_int8_t), 1, file);
}

void assembler(char **instructions, char *filename)
{
  FILE *fileToWriteTo = fopen(filename, "w");
  int i = 0;
  SymbTable table;
  firstPass(&table, instructions);
  u_int32_t binInstruction;
  while (strcmp(instructions[i], "FIN"))
  {
    char **commands = splitUp(instructions[i]);
    MNEMONICS type = getMnemonic(commands);
    switch (type)
    {
    case ADD:
    case SUB:
    case RSB:
    case AND:
    case EOR:
    case ORR:
    case TEQ:
    case MOV:
    case TST:
    case CMP:;
      binInstruction = convertDPToBinary(commands);
      break;
    case MUL:
    case MLA:;
      binInstruction = convertMultiplyToBinary(commands);
      break;
    case LDR:
    case STR:;
      printf("got here\n");

      binInstruction = convertSDTToBinary(commands);
      break;
    case BEQ:
    case BNE:
    case BGE:
    case BLT:
    case BGT:
    case BLE:
    case B:;
      binInstruction = convertBranchToBinary(commands, table, i * 4);
      break;
    case LABEL:
      i++;
      continue;
    case ANDEQ:
      binInstruction = 0;
      break;
    }

    binInstruction = littleEndianConv(binInstruction);
    writeToFile(fileToWriteTo, binInstruction);
    printf("it got to this part");
    i++;
  }

  fclose(fileToWriteTo);
}

/*
Assembler 
takes in : arm source file name, and binary output name
 - creates the binary file equivalent of the arm file
 */
int main(int argc, char **argv)
{

  // check the number of parameters is correct
  if (argc != 3)
  {
    printf("ERROR with arguments: \n");
    printf("argument 1 - Arm Source File, \n Argument 2 - Binary File Name \n");
    return EXIT_FAILURE;
  }

  char *arm_filename = argv[1];
  char **instructionArray = arrayOfArmInstructions(arm_filename, fileLength(arm_filename));
  // Preprocessing for the passes
  // separete out each lines into array of chars for the passes

  // Pass 1 - Symbol Address pairing
  assembler(instructionArray, argv[2]);
  return EXIT_SUCCESS;
}
