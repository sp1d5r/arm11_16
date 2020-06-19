/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * The constants and structs required by the program.
*/

#ifndef boardConstants
#define boardConstant

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define ITEMS_IN_ROW 21
#define ITEMS_IN_ROW 21
#define A_BIG_NUMBER 32000
#define A_SMALL_NUMBER -32000
#define CHECK_IF_NULL(ptr, msg) \
  if (!(ptr))                   \
  perror(msg), exit(EXIT_FAILURE)
#define MAX(a, b) (a < b) ? b : a
#define MIN(a, b) (a < b) ? a : b
#define MAXIMUM_DEPTH 10

typedef enum
{
  TBD,
  DRAWN,
  PLAYERWIN,
  AIWIN
} CELLSTATE;

typedef struct
{
  CELLSTATE state;
  int **boxes;
} cell;

typedef struct board
{
  cell ***cells;
} board;

typedef struct
{
  int score;
  int position;
  int cell;
} potentialPath;

typedef struct
{
  potentialPath **array;
  int total;
} pathArray;

typedef CELLSTATE gameState;

int returnRow(int i);
int returnColumn(int i);
cell *initCell();
potentialPath *initPotentialPath();
pathArray *initPathArray(int length);
board *initBoard();
void deepCopyCell(cell *buffer, cell *actual);
void deepCopyBoard(board *buffer, board *actual);
void freeCell(cell *c);
void freeBoard(board *b);
void freePaths(pathArray *p);
int evaluateBoard(board *b);
int evaluateCell(board *b, int i);
int almostFinishedDiagonals(cell c, bool isPlayer);
int almostFinishedColumns(cell c, bool isPlayer);
int almostFinishedRows(cell c, bool isPlayer);
int returnStateValue(gameState state);
void playerTestPlay(board *b, int cell, int pos);
void AITestPlay(board *b, int cell, int pos);
void resetCell(board *b, int cell, int pos);
void sortPaths(pathArray *p, bool ascending);
void swap(potentialPath *a, potentialPath *b);
bool checkDiagonals(board *b, int cell, bool isPlayer);
bool checkStraightLines(board *b, int cell, bool isPlayer);
bool cellFinished(board *b, int cell, bool isPlayer);
bool cellWon(board *b, int cell);
bool allCellsWon(board *b);
bool gameFinished(board *b, bool isPlayer);
bool gameWon(board *b);
bool isCellFull(cell c);
bool isBoardFull(board b);
gameState returnGameState(board *b);
char cellRep(int i);
char *createRow();
char **createRowArray();
void freeRowArray(char **arr);
void printRow(board *b, int i, char *buffer);
void printBoard(board *b);
void freePick(board *b, int *next, int cell, int position, bool isPlayer);
void restrictedPick(board *b, int *next, int position, bool isPlayer);
int minimaxFree(board *b, bool isMaximising, int alpha, int beta, int depth);
int minimaxRestricted(board *b, bool isMaximising, int cell, int alpha, int beta, int depth);
void askCellandPos(int *, int *);
void askPosition(int *);
void bestMoveFree(board *b, int *cellResult, int *positionResult);
void bestMoveRestricted(board *b, int *positionResult, int cell);
void emptyStdin(void);

#endif // boardConstants
