/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The main file for the assembler.
*/

#include "assembler_utils/assemble.h"

/*
 * SUMMARY: Initialises all values in the state struct.
 *
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: symbolTable table - The symbol table containing the labels and their addresses.
 * PARAMETER: int lineCount - The number of lines in the source file.
 * PARAMETER: char **instructions- An array of strings which stores the lines in the source file.
 *
 * RETURN: void
*/
void initialiseState(instruction *instr,
					 symbolTable table,
					 int lineCount,
					 char **instructions) {
  instr->symbolTable = table;
  instr->lineCount = lineCount;
  instr->lines = instructions;
  instr->sdt_helper.no_instructions = lineCount * 4;
  instr->sdt_helper.finalNumbers = malloc(1 * sizeof(int));
  instr->sdt_helper.sizeOfFinalNumbers = 1;
  *(instr->sdt_helper.finalNumbers) = (-25);
}

/*
 * SUMMARY: Adds a string " end" to the end of each line which assists later.
 *
 * PARAMETER: int lineCount - The number of lines in the source file.
 * PARAMETER: char **instructions - An array of strings which stores the lines in the source file.
 *
 * RETURN: void
*/
void addEndToInstruction(char **instructions, int lineCount) {
  instructions[lineCount][strlen(instructions[lineCount])] = ' ';
  instructions[lineCount][strlen(instructions[lineCount])] = 'e';
  instructions[lineCount][strlen(instructions[lineCount])] = 'n';
  instructions[lineCount][strlen(instructions[lineCount])] = 'd';
}

/*
 * SUMMARY: FirstPass.
 *
 * PARAMETER: symbolTable table - The symbol table which should contain the labels and their addresses.
 * PARAMETER: int *numberOfLines - A pointer to an int which should represent number of lines in the source file.
 * PARAMETER: char *filepath - Path oof the source file.
 *
 * RETURN: char ** - An array of strings which stores the lines in the source file.
*/
char **firstPass(char *filePath, symbolTable *table, int *numberOfLines) {
  FILE *inputFile = fopen(filePath, "r");
  CHECK_IF_NULL(inputFile)
  int labelCount = 0;
  int lineCount = -1;
  char **labels = calloc(0, sizeof(char *));
  int *memoryAddresses = calloc(0, sizeof(int));
  char **instructions = malloc(sizeof(char *));
  instructions[0] = malloc((MAX_LINE_LENGTH + EXTRA_CHARS) * sizeof(char));
  while (fgets(instructions[++lineCount], MAX_LINE_LENGTH, inputFile)) {
	if (instructions[lineCount][0] == '\n') {
	  lineCount--;
	  continue;
	}
	strtok(instructions[lineCount], "\n");
	addEndToInstruction(instructions, lineCount);
	instructions = realloc(instructions, (lineCount + 2) * sizeof(char *));
	instructions[lineCount + 1] = malloc(MAX_LINE_LENGTH + EXTRA_CHARS * sizeof(char));
	if (instructions[lineCount][strlen(instructions[lineCount]) - 5] == ':') {
	  labels = realloc(labels, (labelCount + 1) * sizeof(char *));
	  CHECK_IF_NULL(labels)
	  labels[labelCount] = calloc(MAX_LINE_LENGTH, sizeof(char));
	  CHECK_IF_NULL(labels[labelCount])
	  strcpy(labels[labelCount], strtok(instructions[lineCount], ":"));
	  memoryAddresses = realloc(memoryAddresses, (labelCount + 1) * sizeof(int));
	  CHECK_IF_NULL(memoryAddresses)
	  memoryAddresses[labelCount] = (lineCount * 4) - (4 * labelCount);
	  labelCount++;
	}
  }
  free(instructions[lineCount]);
  instructions = realloc(instructions, (lineCount) * sizeof(char *));
  table->numberOfItems = labelCount;
  table->memoryAddresses = memoryAddresses;
  table->labels = labels;
  fclose(inputFile);
  *numberOfLines = lineCount;
  return instructions;
}

/*
 * SUMMARY: Splits up the string in terms of its operands.
 *
 * PARAMETER: char *instruction = The instruction to be split up.
 *
 * RETURN: char ** - An array of strings which stores the split up operands.
*/
char **splitUp(char *instruction) {
  char clonedValues[MAX_LINE_LENGTH + EXTRA_CHARS];
  strcpy(clonedValues, instruction);
  char **array = (char **)calloc(0, sizeof(char *));
  char *separated_values = strtok(clonedValues, " ,:[]");
  int i = 0;
  while (separated_values) {
	array = realloc(array, (i + 1) * sizeof(char *));
	array[i++] = separated_values;
	separated_values = strtok(NULL, ", .-");
  }
  return array;
}

/*
 * SUMMARY: Gets relevant data from the mnemonicMap.
 *
 * PARAMETER: mnemonicMap map - The mnemonic map.
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 *
 * RETURN: void
*/
void getInstrData(mnemonicMap map, instruction *instr) {
  switch (map.t) {
	case DP:
	  instr->u.opCode = (Dp)map.mnemonic;
	  break;
	case SDT:
	  instr->u.sdt = (Dt)map.mnemonic;
	  break;
	case BRANCH:
	  instr->u.condCode = (Branch)map.mnemonic;
	  break;
	default:
	  break;
  }
}

/*
 * SUMMARY: Converts to little endian format.
 *
 * PARAMETER: uint32_t instruction - The big endian 32 bit instruction.
 *
 * RETURN: uint32_t - Instruction in little endian format.
*/
uint32_t littleEndianConverter(uint32_t instruction) {
  uint32_t firstByte = (instruction & 0xff000000) >> 24;
  uint32_t secondByte = (instruction & 0x00ff0000) >> 8;
  uint32_t thirdByte = (instruction & 0x0000ff00) << 8;
  uint32_t fourthByte = (instruction & 0x000000ff) << 24;
  return firstByte | secondByte | thirdByte | fourthByte;
}

/*
 * SUMMARY: Writes to the outputFile.
 *
 * PARAMETER: FILE *outputFile - The output file.
 * PARAMETER: uint32_t instruction - The instruction to write.
 *
 * RETURN: void
*/
void writeToFile(FILE *outputFile, uint32_t instruction) {
  uint32_t firstByte = (instruction & 0xff000000) >> 24;
  uint32_t secondByte = (instruction & 0x00ff0000) >> 16;
  uint32_t thirdByte = (instruction & 0x0000ff00) >> 8;
  uint32_t fourthByte = (instruction & 0x000000ff);
  fwrite(&firstByte, sizeof(uint8_t), 1, outputFile);
  fwrite(&secondByte, sizeof(uint8_t), 1, outputFile);
  fwrite(&thirdByte, sizeof(uint8_t), 1, outputFile);
  fwrite(&fourthByte, sizeof(uint8_t), 1, outputFile);
}

/*
 * SUMMARY: Decides the type of the instruction using function pointers.
 *
 * PARAMETER: FILE *outputFile - The output file.
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 * PARAMETER: mnemonicMap m[] - An array of mnemonicMaps.
 * PARAMETER: int currentLine - Current line in the array of lines.
 *
 * RETURN: void
*/
void process(int currentLine, mnemonicMap m[], instruction *instr, FILE *outputFile) {
  char *line = malloc(511 * sizeof(char));
  int j;
  strcpy(line, instr->lines[currentLine]);
  strtok(line, " ");
  uint32_t(*func[5])(instruction * ,
  const int) = {
	convertDpToBinary, convertSdtToBinary, convertBranchToBinary, convertMultiplyToBinary, convertSpecialToBinary
  };
  uint32_t binaryInstr = 0;
  for (j = 0; j < NUMBER_OF_COMMANDS; ++j) {
	if (!strcmp(line, m[j].str)) {
	  getInstrData(m[j], instr);
	  binaryInstr = func[m[j].t](instr, currentLine);
	  break;
	}
  }
  if (j == 23) {
	free(line);
	return;
  }
  binaryInstr = littleEndianConverter(binaryInstr);
  writeToFile(outputFile, binaryInstr);
  free(line);
}

/*
 * SUMMARY: Second Pass.
 *
 * PARAMETER: FILE *outputFile - The output file.
 * PARAMETER: instruction *instr - Stores important information about source file instructions.
 *
 * RETURN: void
*/
void secondPass(instruction *state, char *filePath) {
  FILE *binFile = fopen(filePath, "w");
  mnemonicMap m[] =
	  {{ADD, "add", DP}, {SUB, "sub", DP}, {RSB, "rsb", DP}, {AND, "and", DP}, {EOR, "eor", DP},
	   {ORR, "orr", DP}, {MOV, "mov", DP}, {TST, "tst", DP}, {TEQ, "teq", DP}, {CMP, "cmp", DP},
	   {MUL, "mul", MULTIPLY}, {MLA, "mla", MULTIPLY}, {LDR, "ldr", SDT}, {STR, "str", SDT},
	   {BEQ, "beq", BRANCH}, {BNE, "bne", BRANCH}, {BGE, "bge", BRANCH}, {BLT, "blt", BRANCH},
	   {BGT, "bgt", BRANCH}, {BLE, "ble", BRANCH}, {B, "b", BRANCH}, {LSL, "lsl", SPECIAL},
	   {HALT, "andeq", SPECIAL}};
  for (int i = 0; i < state->lineCount; i++) {
	process(i, m, state, binFile);
  }
  int i = 0;
  while (state->sdt_helper.finalNumbers[i] >= 0) {
	// write the binary equivalent of the binary value
	writeToFile(binFile, littleEndianConverter(*(state->sdt_helper.finalNumbers + i)));
	i++;
  }
  fclose(binFile);
}

/*
 * SUMMARY: The main assembler function where everything occurs.
 *
 * PARAMETER: char **argv - The input string array containing input and output file paths.
 *
 * RETURN: void
*/
void assembler(char **argv) {
  symbolTable table;
  table.numberOfItems = 0;
  char **instructions;
  int numberOfLines;
  instructions = firstPass(argv[1], &table, &numberOfLines);
  instruction state1;
  initialiseState(&state1, table, numberOfLines, instructions);
  secondPass(&state1, argv[2]);
  for (int i = 0; i < numberOfLines; i++) {
	free(instructions[i]);
  }
  free(instructions);
  for (int i = 0; i < table.numberOfItems; i++) {
	free(table.labels[i]);
  }
  free(table.labels);
  free(table.memoryAddresses);
  free(state1.sdt_helper.finalNumbers);
}

/*
 * SUMMARY: The main function. It checks if given arguments are valid and then calls the assembler function.
*/
int main(int argc, char **argv) {
  // check the number of parameters is correct
  if (argc != 3) {
	printf("ERROR with arguments:\n");
	printf("Argument 1 - Arm Source File,\nArgument 2 - Binary File Name\n");
	return EXIT_FAILURE;
  }
  assembler(argv);
  return EXIT_SUCCESS;
}