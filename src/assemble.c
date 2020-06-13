#include "assembler_utils/assemble.h"

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
 * arrayOfArmInstructions
 * Params - the filename string, the number of lines in file
 * Returns - the array of arm instructiosn saved in the file
 */
char **arrayOfArmInstructions(char *filename, int lines)
{
  // create a 2d array of instructions
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

void assembler(char **instructions, char *filename, int total_number_instructions)
{
  FILE *fileToWriteTo = fopen(filename, "w");

  // SDT Variables
  total_number_instructions *= 4;
  int *no_instructions = malloc(1 * sizeof(int));
  *no_instructions = total_number_instructions;
  int *finalNumbers = malloc(1 * sizeof(int));
  *finalNumbers = (-25);

  int i = 0;
  int currentInstr = 0;
  SymbTable table;
  firstPass(&table, instructions);
  u_int32_t binInstruction;
  while (strcmp(instructions[i], "FIN"))
  {
    if (strcmp(" end", instructions[i]) == 0)
    {
      i++;
    }
    else
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
        binInstruction = convertSDTToBinary(commands, currentInstr * 4, no_instructions, finalNumbers, 4 * (table.numberOfItems));
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
      case LSL:
        binInstruction = convertLSLToBinary(commands);
        break;
      }

      binInstruction = littleEndianConv(binInstruction);
      writeToFile(fileToWriteTo, binInstruction);
      i++;
      currentInstr++;
    }
  }

  i = 0;
  while (finalNumbers[i] >= 0)
  {
    // write the binarry equivalent of the binary value
    binInstruction = littleEndianConv(*(finalNumbers + i));
    writeToFile(fileToWriteTo, binInstruction);
    i++;
  }

  // take each number and add to end of while
  // i last instruciton

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

  int total_instructions = 0;

  for (int i = 0; i < fileLength(arm_filename); i++)
  {
    if (operandTotal(splitUp(instructionArray[i])) != 0)
    {
      total_instructions++;
    }
  }

  // Pass 1 - Symbol Address pairing
  assembler(instructionArray, argv[2], total_instructions);
  return EXIT_SUCCESS;
}