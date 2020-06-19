/*
 * GROUP 16 - Members: Aayush, Ayoob, Devam, Elijah
 * Functions that make a mark on the board.
*/

#include "ultimateTicTacToe.h"

//this is how a free pick is done - the position is stored in next so the next cell can be picked
void freePick(board *b, int *next, int cell, int position, bool isPlayer)
{
  if (position > 8 || cell > 8)
  {
    printf("Out of bounds. Please select a cell number and position inside the cell between 0 and 8");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  else if (b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(position)][returnColumn(position)] != 0)
  {
    printf("Already written in this position, pick a cell and position again\n");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  else if (cellWon(b, cell))
  {
    printf("This cell has already been won, pick a different cell and position (separated by a space)");
    scanf("%i", &cell);
    scanf("%i", &position);
    freePick(b, next, cell, position, isPlayer);
  }
  else
  {
    *next = position;
    b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(position)][returnColumn(position)] = isPlayer ? 1 : 2;
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
    printf("Out of bounds. Please select a position inside between 0 and 8");
    scanf("%i", &position);
    restrictedPick(b, next, position, isPlayer);
  }
  else if (b->cells[returnRow(*next)][returnColumn(*next)]->boxes[returnRow(position)][returnColumn(position)] != 0)
  {
    printf("Already written in this position, pick a position again\n");
    scanf("%i", &position);
    restrictedPick(b, next, position, isPlayer);
  }
  else if (cellWon(b, *next))
  {
    int cell;
    printf("This cell has already been won. You get a free pick! Pick a different cell and position (separated by a space)\n");
    scanf("%i", &position);
    scanf("%i", &cell);
    freePick(b, next, cell, position, isPlayer);
  }
  else
  {
    b->cells[returnRow(*next)][returnColumn(*next)]->boxes[returnRow(position)][returnColumn(position)] = isPlayer ? 1 : 2;
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
}
