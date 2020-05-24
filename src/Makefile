CC      = gcc
CFLAGS  = -Wall -g -D_POSIX_SOURCE -D_BSD_SOURCE -std=c99 -Werror -pedantic

.SUFFIXES: .c .o

.PHONY: all clean

all: assemble emulate

emulate: emulate.c
	CC -g -Wall emulate.c -o emulate

assemble: assemble.o

emulate: emulate.o

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
