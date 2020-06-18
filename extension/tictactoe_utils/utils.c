// STRUCT BASED FUNCTIONS -- INITIALISE AND FREE

#include "ultimateTicTacToe.h"

int returnRow(int i)
{
	return i / 3;
}

int returnColumn(int i)
{
	return i % 3;
}

// intialises a cell and puts it in the heap
cell *initCell()
{
	cell *new = (cell *)calloc(1, sizeof(cell));
	NULL_CHECK(new, "Couldn't allocate a cell.\n");
	new->state = TBD;
	new->boxes = (int **)calloc(3, sizeof(int *));
	NULL_CHECK(new->boxes, "Couldn't allocate the array of int arrays.\n");
	for (int i = 0; i < 3; i++)
	{
		new->boxes[i] = (int *)calloc(3, sizeof(int));
		NULL_CHECK(new->boxes[i], "Couldn't allocate the array of 3 ints.\n");
	}
	return new;
}

potentialPath *initPotentialPath()
{
	potentialPath *new = (potentialPath *)calloc(1, sizeof(potentialPath));
	NULL_CHECK(new, "Couldn't allocate a new path.\n");
	return new;
}

pathArray *initPathArray(int length)
{
	pathArray *new = (pathArray *)calloc(1, sizeof(pathArray));
	NULL_CHECK(new, "Couldn't alloate new path array.\n");
	potentialPath **array = (potentialPath **)calloc(length, sizeof(potentialPath *));
	NULL_CHECK(new, "Couldn't allocate array.\n");
	for (int i = 0; i < length; i++)
	{
		array[i] = initPotentialPath();
	}
	new->array = array;
	new->total = length;
	return new;
}

// intiliases the board and all of its cells
board *initBoard()
{
	board *new = (board *)calloc(1, sizeof(board));
	NULL_CHECK(new, "Couldn't allocate the board.\n");
	new->cells = (cell ***)calloc(3, sizeof(cell **));
	NULL_CHECK(new->cells, "Couldn't allocate cell.\n");
	for (int i = 0; i < 3; i++)
	{
		new->cells[i] = (cell **)calloc(3, sizeof(cell *));
		NULL_CHECK(new->cells[i], "Couldn't allocate the row of cells\n");
	}
	for (int j = 0; j < 9; j++)
	{
		new->cells[returnRow(j)][returnColumn(j)] = initCell();
	}
	return new;
}

//creates a  copy of a cell
void deepCopyCell(cell *buffer, cell *actual)
{
	buffer->state = actual->state;
	for (int i = 0; i < 9; i++)
	{
		buffer->boxes[returnRow(i)][returnColumn(i)] = actual->boxes[returnRow(i)][returnColumn(i)];
	}
}

//creates a copy of a whole board (ONLY USED IN BEST MOVE FUNCTIONS)
void deepCopyBoard(board *buffer, board *actual)
{
	for (int i = 0; i < 9; i++)
	{
		deepCopyCell(buffer->cells[returnRow(i)][returnColumn(i)],
			actual->cells[returnRow(i)][returnColumn(i)]);
	}
}

//frees a cell from memory
void freeCell(cell *c)
{
	for (int i = 0; i < 3; i++)
	{
		free(c->boxes[i]);
	}
	free(c->boxes);
	free(c);
}

//frees the board from memory
void freeBoard(board *b)
{
	for (int i = 0; i < 9; i++)
	{
		freeCell(b->cells[returnRow(i)][returnColumn(i)]);
	}
	free(b->cells);
	free(b);
}

void freePaths(pathArray *p)
{
	for (int i = 0; i < p->total; i++)
	{
		free(p->array[i]);
	}
	free(p);
}