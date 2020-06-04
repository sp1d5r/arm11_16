CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

assemble: assemble.o
		$(CC) $^ -o $@

emulate: emulate.o emulator_utils/pipeline.o emulator_utils/output.o emulator_utils/executeHelper.o emulator_utils/executeInstr.o emulator_utils/shiftInstructions.o emulator_utils/decodeInstr.o
		$(CC) $^ -o $@

assemble.o: assemble.c
		$(CC) $(CFLAGS) $< -c

emulate.o: emulate.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

pipeline.o: emulator_utils/pipeline.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

shiftInstructions.o : emulator_utils/shiftInstructions.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

executeInstr.o : emulator_utils/executeInstr.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

branchAndMultiply.o: emulator_utils/branchAndMultiply.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

output.o : emulator_utils/output.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

dataTransfer.o : emulator_utils/dataTransfer.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

dataProcess.o : emulator_utils/dataProcess.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

decodeInstr.o : emulator_utils/decodeInstr.c emulator_utils/defines.h
		$(CC) $(CFLAGS) $< -c

clean:
	rm -f $(wildcard *.o)
	rm -f emulate
	rm -f assemble
	rm -f emulator_utils/*.o
	rm -f emulate.o
