CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_POSIX_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o assembler_utils/processSDT.o assembler_utils/processMul.o assembler_utils/processSpecial.o assembler_utils/processHelpers.o assembler_utils/processDP.o assembler_utils/processBranch.o
		$(CC) $^ -o $@

emulate: emulate.o emulator_utils/pipeline.o emulator_utils/output.o emulator_utils/executeHelper.o emulator_utils/executeInstr.o emulator_utils/shiftInstructions.o emulator_utils/decodeInstr.o
		$(CC) $^ -o $@

assemble.o: assemble.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

emulate.o: emulate.c emulator_utils/emulateConstants.h
		$(CC) $(CFLAGS) $< -c

pipeline.o: emulator_utils/pipeline.c emulator_utils/emulateConstants.h emulator_utils/pipelineConstants.h
		$(CC) $(CFLAGS) $< -c

shiftInstructions.o : emulator_utils/shiftInstructions.c emulator_utils/emulateConstants.h emulator_utils/shiftingConstants.h
		$(CC) $(CFLAGS) $< -c

executeInstr.o : emulator_utils/executeInstr.c emulator_utils/emulateConstants.h emulator_utils/executeConstants.h
		$(CC) $(CFLAGS) $< -c

output.o : emulator_utils/output.c emulator_utils/emulateConstants.h emulator_utils/outputConstants.h
		$(CC) $(CFLAGS) $< -c

decodeInstr.o : emulator_utils/decodeInstr.c emulator_utils/emulateConstants.h emulator_utils/decodeConstants.h
		$(CC) $(CFLAGS) $< -c

executeHelper.o : emulator_utils/executeInstr.c emulator_utils/emulateConstants.h emulator_utils/executeConstants.h
		$(CC) $(CFLAGS) $< -c		

processBranch.o : assembler_utils/processBranch.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

processDP.o : assembler_utils/processDP.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

processHelpers.o : assembler_utils/processHelpers.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

processSpecial.o : assembler_utils/processSpecial.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

processMul.o : assembler_utils/processMul.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

processSDT.o : assembler_utils/processSDT.c assembler_utils/assemble.h
		$(CC) $(CFLAGS) $< -c

clean:
	rm -f $(wildcard *.o)
	rm -f emulate
	rm -f assemble
	rm -f emulator_utils/*.o
	rm -f assembler_utils/*.o
