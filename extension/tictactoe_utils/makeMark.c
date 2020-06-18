#include "ultimateTicTacToe.h"

//FUNCTIONS THAT MAKE A MARK ON THE BOARD

//this is how a free pick is done - the position is stored in next so the next cell can be picked
void freePick(board *b, int *next, int cell, int position, bool isPlayer)
{
  if (position > 8 || cell > 8)
  {
    printf(
        "Out of bounds. Please select a cell number and position inside the cell between 0 and 8.\n");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  if (b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(position)][returnColumn(
          position)] != 0)
  {
    printf("Already written in this position. Please pick an empty cell and position again.\n");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  if (cellWon(b, cell))
  {
    printf("This cell has already been won, please pick another cell.\n");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  else
  {
    *next = position;
    b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(position)][returnColumn(
        position)] = isPlayer ? 1 : 2;
    if (cellFinished(b, cell, isPlayer))
    {
      b->cells[returnRow(cell)][returnColumn(cell)]->state = isPlayer ? PLAYERWIN : AIWIN;
    }
  }
}

void restrictedPick(board *b, int *next, int position, bool isPlayer)
{
  if (position > 8)
  {
    printf(
        "Out of bounds. Please select a cell number and position inside the cell between 0 and 8.\n");
    scanf("%i", &position);
    restrictedPick(b, next, position, isPlayer);
  }
  if (b->cells[returnRow(*next)][returnColumn(*next)]->boxes[returnRow(position)][returnColumn(
          position)] != 0)
  {
    printf("Already written in this poition. Please pick an empty position again.\n");
    scanf("%i", &position);
    restrictedPick(b, next, position, isPlayer);
  }
  if (cellWon(b, *next))
  {
    int cell;
    printf("This cell has already been won. Please pick a different one and a position.\n");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  b->cells[returnRow(*next)][returnColumn(*next)]->boxes[returnRow(position)][returnColumn(
      position)] = isPlayer ? 1 : 2;
  if (cellFinished(b, *next, isPlayer))
  {
    b->cells[returnRow(*next)][returnColumn(*next)]->state = isPlayer ? PLAYERWIN : AIWIN;
  }
  else if (isCellFull(*b->cells[returnRow(*next)][returnColumn(*next)]))
  {
    b->cells[returnRow(*next)][returnColumn(*next)]->state = DRAWN;
  }
  *next = position;
}