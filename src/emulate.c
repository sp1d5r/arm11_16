/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The main file where reading from a binary file is done and instructions are given to the
 * pipeline to process.
*/

#include "emulator_utils/emulateConstants.h"

void initialiseState(ARMSTATE *);
bool isFileNull(FILE *);
int readFromBinFile(ARMSTATE *, FILE *);

/*
 * SUMMARY: Initialises all registers and memory addresses to zero.
 * Also initialises the fetch and decode booleans to zero.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 *
 * RETURN: void
*/

void initialiseState(ARMSTATE *state)
{
	for (int i = 0; i < NUMBER_OF_REGISTERS; ++i)
		state->regs[i] = INIT_ZERO_VAL;
	state->memory = (uint8_t *)calloc(MAX_BYTES, sizeof(uint8_t));
	state->fetchDecodeExecute[0] = state->fetchDecodeExecute[1] = false;
}

/*
 * SUMMARY: Checks if the input file is null.
 *
 * PARAMETER: FILE *binaryFile - Pointer to the binary file given in.
 *
 * RETURN: bool - true if null otherwise false.
*/

bool isFileNull(FILE *binaryFile)
{
	if (binaryFile == NULL)
	{
		printf("Please use a valid file.\n");
		return EXIT_FAILURE;
	}
	return 0;
}

/*
 * SUMMARY: Reads from the binary file.
 *
 * PARAMETER: ARMSTATE *state - The current state of the machine.
 * PARAMETER: FILE *binaryFile - Pointer to the binary file given in.
 *
 * RETURN: int
*/

int readFromBinFile(ARMSTATE *state, FILE *binaryFile)
{

	int arrayIndex = -1;
	if (state->memory == NULL)
		return ALLOCATION_ERROR;
	while ((fread(&state->memory[++arrayIndex], sizeof(uint8_t), 1, binaryFile) == 1))
	{
	}
	fclose(binaryFile);
	return arrayIndex;
}

/*
 * SUMMARY: Reading from a bin file is done and instructions are given to the pipeline
 * to process.
*/

int main(int argc, char **argv)
{
	if (argc != NUMBER_OF_ARGUMENTS)
		return EXIT_FAILURE;
	ARMSTATE state;
	FILE *binaryFile;
	binaryFile = fopen(argv[ARG_WITH_FILE], "rb");
	if (isFileNull(binaryFile))
		return EXIT_FAILURE;
	initialiseState(&state);
	int noOfBytesInFile = readFromBinFile(&state, binaryFile);
	if (noOfBytesInFile == ALLOCATION_ERROR)
	{
		free(state.memory);
		return EXIT_FAILURE;
	}
	pipeline(&state);
	free(state.memory);
	return EXIT_SUCCESS;
}