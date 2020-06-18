#ifndef board
#define board

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define ITEMS_IN_ROW 21
#define FILECHECK(ptr, msg) if (!(ptr)) perror(msg), exit(EXIT_FAILURE)

typedef enum{
  TBD,DRAWN,PLAYERWIN,AIWIN
} CELLSTATE;

typedef struct{
  CELLSTATE state;
  int **boxes;
} cell;

typedef struct board{
  cell **cells;
} board;

#endif
