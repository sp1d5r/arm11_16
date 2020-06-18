#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#define ITEMS_IN_ROW 21
#define A_BIG_NUMBER 32000
#define A_SMALL_NUMBER -32000
#define FILECHECK(ptr, msg) if (!(ptr)) perror(msg), exit(EXIT_FAILURE)
#define MAX(a,b) (a<b) ?  b : a
#define MIN(a,b) (a<b) ?  a : b
#define MAXIMUM_DEPTH 17

typedef enum{
  TBD,DRAW,PLAYERWIN,AIWIN
} CELLSTATE;

typedef struct{
  CELLSTATE state;
  int **boxes;
} cell;

typedef struct board{
  cell ***cells;
} board;

typedef struct{
  int score;
  int position;
  int cell;
} potentialPath;

typedef struct{
  potentialPath **array;
  int total;
} pathArray;

typedef CELLSTATE gameState;

int returnRow(int i){
  return i / 3;
}

int returnColumn(int i){
  return i % 3;
}


// STRUCT BASED FUNCTIONS -- INITILIASE AND FREE
// intialises a cell and puts it in the heap
cell *initCell(){
  cell *new = (cell*) calloc(1,sizeof(cell));
  FILECHECK(new,"couldn't allocate cell");
  new -> state = TBD;
  new -> boxes = (int**) calloc(3,sizeof(int*));
  FILECHECK(new -> boxes,"couldn't allocate array of int arrays");
  for (int i = 0; i < 3; i++){
    new -> boxes[i] = (int*) calloc(3,sizeof(int));
    FILECHECK(new->boxes[i],"couldn't allocate array of 3 ints");
  }
  return new;
}

potentialPath *initPotentialPath(){
  potentialPath *new = (potentialPath*) calloc(1,sizeof(potentialPath));
  FILECHECK(new,"couldnt allocate new path");
  return new;
}

pathArray *initPathArray(int length){
  pathArray *new = (pathArray*) calloc(1,sizeof(pathArray));
  FILECHECK(new,"couldnt alloate new path array");
  potentialPath **array = (potentialPath**) calloc(length,sizeof(potentialPath*));
  FILECHECK(new,"couldnt allocate array");
  for (int i = 0; i < length; i++){
    array[i] = initPotentialPath();
  }
  new -> array = array;
  new -> total = length;
  return new;
}

// intiliases the board and all of its cells
board *initBoard(){
  board *new = (board*) calloc(1,sizeof(board));
  FILECHECK(new,"couldn't allocate board");
  new -> cells = (cell***) calloc(3,sizeof(cell**));
  FILECHECK(new->cells,"couldn't allocate cell");
  for (int i = 0; i <  3; i++){
    new -> cells[i] = (cell**) calloc(3,sizeof(cell*));
     FILECHECK(new->cells[i],"couldn't allocate row of cells");
  }
  for (int j = 0; j < 9; j++){
    new -> cells[returnRow(j)][returnColumn(j)] = initCell();
  }
  return new;
}


//creates a  copy of a cell
void deepCopyCell(cell *buffer, cell *actual){
  buffer -> state = actual -> state;
  for (int i = 0; i < 9; i ++){
    buffer -> boxes[returnRow(i)][returnColumn(i)] =  actual -> boxes[returnRow(i)][returnColumn(i)];
  }
}

//creates a copy of a whole board (ONLY USED IN BEST MOVE FUNCTIONS)
void deepCopyBoard(board *buffer, board *actual){
  for (int i = 0; i < 9; i ++){
    deepCopyCell(buffer -> cells[returnRow(i)][returnColumn(i)],actual -> cells[returnRow(i)][returnColumn(i)]);
  }
}


//frees a cell from memory
void freeCell(cell *c){
  for (int i =0; i < 3; i++){
    free(c -> boxes[i]);
  }
  free(c -> boxes);
  free(c);
}

//frees the board from memory
void freeBoard(board *b){
  for (int i = 0; i < 9; i++){
    freeCell(b -> cells[returnRow(i)][returnColumn(i)]);
  }
  free(b -> cells);
  free(b);
}

void freePaths(pathArray *p){
  for (int i = 0; i < p->total; i++){
    free(p->array[i]);
  }
  free(p);
}
// ----------------------------------------------------------------------//

// FUNCTIONS THAT REPRESENT THE BOARD IN TERMINAL

//converts an integer into a character representation
char cellRep(int i){
  switch (i){
  case 1:
    return 'X';
    break;
  case 2:
    return 'O';
    break;
  default:
    return '-';
    break;
  }
}


//creates a string and puts it on the heap
char *createRow(){
  char *new = (char*) calloc(ITEMS_IN_ROW, sizeof(char));
  FILECHECK(new,"couldn't allocate string");
  return new;
}
//creates an array of strings and puts them on the heap
char **createRowArray(){
  char **new = (char**) calloc(9, sizeof(char*));
  FILECHECK(new,"couldn't allocate string array");
  for (int i = 0; i < 9; i++){
    new[i] = createRow();
  }
  return new;
}

//frees the created string array
void freeRowArray(char **arr){
  for (int i = 0; i < 9; i ++){
    free(arr[i]);
  }
  free(arr);
}


//converts a whole row of the board into a string and stores it in a string buffer
void printRow(board *b, int i, char *buffer){
  int cellRow = i < 3 ? 0 : (i < 6 ? 1: 2);
  int rowInCell  = i % 3;
  int counter = 0;
  cell **rowOfCells = b -> cells[cellRow];
  cell *currCell;
  for (int i =0; i < 3; i++){
    currCell = rowOfCells[i];
    for (int j = 0; j < 3; j++){
      buffer[counter] = cellRep(currCell->boxes[rowInCell][j]);
      counter++;
      if (j != 2){
	buffer[counter] = '|';
	counter++;
      } else {
	buffer[counter] = ' ';
	buffer[counter + 1] = ' ';
	counter += 2;
      }
    }
  }

}


//prints out the whole board
void printBoard(board *b){
  char **rowArr = createRowArray();
  for (int i = 0; i < 9; i++){
    printRow(b,i,rowArr[i]);
  }
 
  for (int j = 0; j < 9; j++){
    printf("%s",rowArr[j]);
    if (j  == 2 || j == 5){
      printf("\n");
    }
    printf("\n");
  }
  freeRowArray(rowArr);

}


//-----------------------------------------------------------------------
//FUNCTIONS THAT GET THE STATE OF THE GAME

//this is to check that the diagonal is one player - if cell is -1 it is looking at the whole board and if it is a normal number then it is looking at the cell it describes
bool checkDiagonals(board *b, int cell, bool isPlayer){
  bool diag1 = true;
  bool diag2 = true;
  if  (cell == -1){
    CELLSTATE targetState = isPlayer ? PLAYERWIN : AIWIN;
    for (int i = 0; i < 3; i++){
      diag1 &= b->cells[i][i]->state == targetState;
      diag2 &= b->cells[i][2-i]->state == targetState;
    }
    return diag1 || diag2;
  } else {
    int targetValue = isPlayer ? 1 : 2;
    for (int i = 0; i < 3; i++){
      diag1 &= b->cells[returnRow(cell)][returnColumn(cell)]->boxes[i][i] == targetValue;
      diag2 &= b->cells[returnRow(cell)][returnColumn(cell)]->boxes[i][2-i] == targetValue;
    }
    return diag1 || diag2;
  }
}

bool checkStraightLines(board *b, int cell, bool isPlayer){
  bool horizontal = true;
  bool vertical  = true;
  if (cell == -1){
    CELLSTATE targetState = isPlayer ? PLAYERWIN : AIWIN;
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){
	horizontal &= b -> cells[i][j] -> state == targetState;
	vertical   &= b -> cells[j][i] -> state == targetState;
      }
      if (horizontal || vertical){
	return true;
      }
      horizontal = true;
      vertical = true;
    }
    return false;
  } else {   
    int targetValue = isPlayer ? 1 : 2;
    for (int i = 0; i < 3; i++){
      for (int j = 0; j < 3; j++){
	horizontal &= b -> cells[returnRow(cell)][returnColumn(cell)]->boxes[i][j] == targetValue;
	vertical   &= b -> cells[returnRow(cell)][returnColumn(cell)]->boxes[j][i] == targetValue;
      }
      if (horizontal || vertical){
	return true;
      }
      horizontal = true;
      vertical = true;
    }
    return false;
 
  }



}


//sees if a cell has already been won by either the AI or player
bool cellFinished(board *b, int cell ,bool isPlayer){
  bool diagonals = checkDiagonals(b,cell,isPlayer);
  bool straightLines = checkStraightLines(b,cell,isPlayer);
  if (diagonals || straightLines){
    if ( b->cells[returnRow(cell)][returnColumn(cell)]->state != TBD){
      return true;
    }
    b->cells[returnRow(cell)][returnColumn(cell)]->state = isPlayer ? PLAYERWIN : AIWIN;
    return true;
  } else {
    b->cells[returnRow(cell)][returnColumn(cell)]->state = TBD;
    return false;
  }
}

//sees if a player has won the cell so no one should play in it
bool cellWon(board *b, int cell){
  return cellFinished(b,cell,true) || cellFinished(b,cell,false);
}

//checks if all cells have been won
bool allCellsWon(board *b){
  for (int i = 0; i < 9; i++){
    if (!cellWon(b,i)) return false;
  }
  return true;
}

//sees if the game has been won by a certain player
bool gameFinished(board *b, bool isPlayer){
  bool diagonals = checkDiagonals(b,-1,isPlayer);
  bool straightLines = checkStraightLines(b,-1,isPlayer);
  return diagonals || straightLines;
}
  
//sees if someone has won the game
bool gameWon(board *b){
  return gameFinished(b,true) || gameFinished(b,false);
}

//checks if a cell is completely filled
bool isCellFull(cell c){
  for (int i = 0; i < 3; i++){
    for (int j = 0; j < 3; j++){
      if (c.boxes[i][j] == 0){
	return false;
      }
    }
  }
  return true;
}
//checks if there are no more places to play on the board
bool isBoardFull(board b){
  for (int i = 0; i < 9; i++){
    if (!isCellFull(*b.cells[returnRow(i)][returnColumn(i)])){
	return false;
    }
  }
  return true;
}

//returns the state of the game
gameState returnGameState(board *b){
  if (gameFinished(b,true)) return PLAYERWIN;
  else if (gameFinished(b,false)) return AIWIN;
  else if (isBoardFull(*b)) return DRAW;
  else return TBD;
}

//----------------------------------------------------------------------
//FUNCTIONS THAT MAKE A MARK ON THE BOARD


//this is how a free pick is done - the position is stored in next so the next cell can be picked
void freePick(board *b, int *next, int cell, int position, bool isPlayer){
  if (position > 8 || cell > 8){
    printf("out of bounds. Please select a cell number and position inside the cell between 0 and 8");
    scanf("%i",&cell);
    scanf("%i", &position);
     freePick(b,next,cell,position,isPlayer);
  } 
  if (b -> cells[returnRow(cell)][returnColumn(cell)] -> boxes[returnRow(position)][returnColumn(position)] != 0){
    printf("already written in this poition, pick a cell and position again\n");
    scanf("%i",&cell);
    scanf("%i", &position);
    freePick(b,next,cell,position,isPlayer);
  }
  if (cellWon(b,cell)){
    printf("this cell has already been won, please pick another cell");
    scanf("%i",&cell);
    scanf("%i", &position);
    freePick(b,next,cell,position,isPlayer);
  } else {
  *next = position;
  b -> cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(position)][returnColumn(position)] = isPlayer ? 1 : 2;
  if (cellFinished(b,cell,isPlayer)){
    b->cells[returnRow(cell)][returnColumn(cell)]->state = isPlayer ? PLAYERWIN:AIWIN;
  }
}
}


void restrictedPick(board *b, int *next, int position, bool isPlayer){
  if (position > 8){
    printf("out of bounds. Please select a cell number and position inside the cell between 0 and 8");
    scanf("%i", &position);
    restrictedPick(b,next,position,isPlayer);
  }
  if (b -> cells[returnRow(*next)][returnColumn(*next)] -> boxes[returnRow(position)][returnColumn(position)] != 0){
    printf("already written in this poition, pick a position again\n");
    scanf("%i", &position);
    restrictedPick(b,next,position,isPlayer);
  }
  if (cellWon(b,*next)){
    int cell;
    printf("this cell has already been won, pick a different one and a position");
    scanf("%i",&cell);
    scanf("%i", &position);
    freePick(b,next,cell,position,isPlayer);
  }
  b -> cells[returnRow(*next)][returnColumn(*next)]->boxes[returnRow(position)][returnColumn(position)] = isPlayer ? 1 : 2;
  if (cellFinished(b,*next,isPlayer)){
    b->cells[returnRow(*next)][returnColumn(*next)]->state = isPlayer ? PLAYERWIN:AIWIN;
  } else if (isCellFull(*b->cells[returnRow(*next)][returnColumn(*next)])){
      b->cells[returnRow(*next)][returnColumn(*next)]->state = DRAW;
    }
  *next = position;
}
  
//-------------------------------------------------------------------

//FUNCTIONS USED IN THE AI OF THE GAME

void swap(potentialPath *a, potentialPath *b){
  potentialPath temp = *a;
  *a = *b;
  *b = temp;
}

void sortPaths(pathArray *p, bool ascending){
  int size = p->total;
  if (ascending){
  for (int i = 0; i < size; i++){
    for (int j = i + 1; j < size; j++){
      if (p->array[i]->score < p->array[j]->score){
	swap(p->array[i],p->array[j]);
      }
    }
  }
  } else {
    for (int i = 0; i < size; i++){
    for (int j = i + 1; j < size; j++){
      if (p->array[i]->score > p->array[j]->score){
	swap(p->array[i],p->array[j]);
      }
    }
  }

  }
}

//resets a cell (ONLY USED IN MINIMAX FUNCTIONS)
void resetCell(board *b, int cell, int pos){
  b -> cells[returnRow(cell)][returnColumn(cell)] -> boxes[returnRow(pos)][returnColumn(pos)] = 0;
}

// sets a cell to an AI play (ONLY USED IN MINIMAX FUNCTIONS)
void AITestPlay(board *b, int cell, int pos){
   b -> cells[returnRow(cell)][returnColumn(cell)] -> boxes[returnRow(pos)][returnColumn(pos)] = 2;
}

// sets a cell to an user play (ONLY USED IN MINIMAX FUNCTIONS)
void playerTestPlay(board *b, int cell, int pos){
   b -> cells[returnRow(cell)][returnColumn(cell)] -> boxes[returnRow(pos)][returnColumn(pos)] = 1;
}


//returns an integer value of the state of the game
int returnStateValue(gameState state){
  if (state == AIWIN) return 100;
  if (state == PLAYERWIN) return -100;
  else return 0;
}

//works out how many almost completed rows there are for either the CPU or player
int almostFinishedRows(cell c, bool isPlayer){
  int targetValue = isPlayer ? 1 : 2;
  int result = 0;
  for (int i = 0; i < 3; i++){
    bool pair1 = c.boxes[i][0] == c.boxes[i][1] && c.boxes[i][0] == targetValue && c.boxes[i][2] == 0;
    bool pair2 = c.boxes[i][0] == c.boxes[i][2] && c.boxes[i][0] == targetValue && c.boxes[i][1] == 0;
    bool pair3 = c.boxes[i][1] == c.boxes[i][2] && c.boxes[i][1] == targetValue && c.boxes[i][0] == 0;
    if (pair1 || pair2 || pair3){
      result++;
    }
  }
  return result;
}
// works out how many almost completed columns there are for either CPU or player
int almostFinishedColumns(cell c, bool isPlayer){
  int targetValue = isPlayer ? 1 : 2;
  int result = 0;
  for (int i = 0; i < 3; i++){
    bool pair1 = c.boxes[0][i] == c.boxes[1][i] && c.boxes[1][i] == targetValue && c.boxes[2][i] == 0;
    bool pair2 = c.boxes[0][i] == c.boxes[2][i] && c.boxes[0][i] == targetValue && c.boxes[1][i] == 0;
    bool pair3 = c.boxes[1][i] == c.boxes[2][i] && c.boxes[1][i] == targetValue && c.boxes[0][i] == 0;
    if (pair1 || pair2 || pair3){
      result++;
    }
  }
  return result;
}
//sees if there are any almost finished diagonals for eith er the CPU or player
int almostFinishedDiagonals(cell c, bool isPlayer){
  int targetValue = isPlayer ? 1 : 2;
  int result = 0;
  bool diag1 = c.boxes[0][0] == c.boxes[1][1] && c.boxes[0][0] == targetValue && c.boxes[2][2] == 0;
  bool diag2 = c.boxes[0][0] == c.boxes[2][2] && c.boxes[0][0] == targetValue && c.boxes[1][1] == 0;
  bool diag3 = c.boxes[1][1] == c.boxes[2][2] && c.boxes[1][1] == targetValue && c.boxes[0][0] == 0;
  if (diag1 || diag2 || diag3){
    result++;
  }
  diag1 = c.boxes[0][2] == c.boxes[1][1] && c.boxes[0][2] == targetValue && c.boxes[2][0] == 0;
  diag2 = c.boxes[0][2] == c.boxes[2][0] && c.boxes[0][2] == targetValue && c.boxes[1][1] == 0;
  diag3 = c.boxes[1][1] == c.boxes[2][0] && c.boxes[1][1] == targetValue && c.boxes[0][2] == 0;
  if (diag1 || diag2 || diag3){
    result++;
  }
  return result;
}
      

//returns an int for how "good" the board is for the AI
int evaluateCell(board *b, int i){
  cell c = *b->cells[returnRow(i)][returnColumn(i)];
  if (b->cells[returnRow(i)][returnColumn(i)] ->state == AIWIN) return 10;
  if (b->cells[returnRow(i)][returnColumn(i)] -> state == PLAYERWIN) return -10;
  if (isCellFull(*b->cells[returnRow(i)][returnColumn(i)])) return 0;
  int AIPairs = 2*(almostFinishedRows(c,false) + almostFinishedColumns(c,false) + almostFinishedDiagonals(c,false));
  int playerPairs = 2*(almostFinishedRows(c,true) + almostFinishedColumns(c,true) + almostFinishedDiagonals(c,true));
  
  int powerPositions = 0;
  if (c.boxes[1][1] == 1) powerPositions -= 2;
  if (c.boxes[1][1] == 2) powerPositions += 2;
  for (int k =0; k < 3; k++){
    if (k%2==0){
    if (c.boxes[k][k] == 2) powerPositions++;
    if (c.boxes[k][k] == 1) powerPositions--;
    if (c.boxes[k][2-k] == 1) powerPositions--;
    if (c.boxes[k][2-k] == 2) powerPositions++;
    }
  }
  return AIPairs - playerPairs + powerPositions;
  
}

int evaluateBoard(board *b){
  int result = 0;
  for (int i = 0; i < 9; i++){
    result += evaluateCell(b,i);
  }
  return result;
}

int minimaxRestricted(board *b, bool , int, int , int, int );

//the minimax algorithm when the pick is free
int minimaxFree(board *b, bool isMaximising, int alpha, int beta, int depth){
  gameState currState = returnGameState(b);
  int score;
  int childScore;
  if (currState != TBD){
    int value = returnStateValue(currState);
    return value - depth;
  }
  if (isBoardFull(*b) || allCellsWon(b)){
    return -depth;
  }

  
  if (depth == MAXIMUM_DEPTH){
    return evaluateBoard(b);
  }
  
   int potentialPaths = 0;
   for (int i = 0; i < 9; i++){
     for (int j = 0; j < 9; j++){
       if (b->cells[returnRow(i)][returnColumn(i)]->boxes[returnRow(j)][returnColumn(j)] == 0){
	   potentialPaths++;
	 }
     }
   }

   pathArray *paths = initPathArray(potentialPaths);
   int pathCounter = 0;
   for (int k = 0; k < 9; k++){
     for (int l = 0; l < 9; l++){
       if (b->cells[returnRow(k)][returnColumn(k)]->boxes[returnRow(l)][returnColumn(l)] == 0){
	 if (isMaximising){
	   AITestPlay(b,k,l);
	 } else {
	   playerTestPlay(b,k,l);
	 }
	 paths->array[pathCounter]->score = evaluateBoard(b);
	 paths->array[pathCounter]->position = l;
	 paths->array[pathCounter]->cell = k;
	 pathCounter++;
	 resetCell(b,k,l);
       }
     }
   }

   sortPaths(paths,isMaximising);
   int halfOfPaths = paths->total / 2;

  if (isMaximising){

    for (int i = 0; i < halfOfPaths; i++){
      int cell = paths->array[i]->cell;
      int position = paths -> array[i] -> position;
      AITestPlay(b,cell,position);
      if (!cellWon(b,position) && !isCellFull(*b->cells[returnRow(position)][returnColumn(position)])){
	childScore = minimaxRestricted(b,false,position,alpha,beta,depth+1);
      } else {
	childScore = minimaxFree(b,false,alpha,beta,depth+1);
      }
      resetCell(b,cell,position);
      score = MAX(score,childScore);
      alpha = MAX(score,alpha);
      if (alpha <= beta){
	break;
      }
    }
    freePaths(paths);
    return score;
  } else {
    for (int i = 0; i < halfOfPaths; i++){
      int cell = paths->array[i]->cell;
      int position = paths->array[i]->position;
      playerTestPlay(b,cell,position);
       if (!cellWon(b,position) && !isCellFull(*b->cells[returnRow(position)][returnColumn(position)])){
	 childScore = minimaxRestricted(b,true,position,alpha,beta,depth+1);
       } else {
	 childScore = minimaxFree(b,true,alpha,beta,depth+1);
       }
       resetCell(b,cell,position);
       score = MIN(score,childScore);
       beta = MIN(score,beta);
       if (alpha <= beta){
	 break;
       }
    }
    freePaths(paths);
    return score;
  }
  
}

 
//the minimax algorithm when the pick is restricted
int minimaxRestricted(board *b, bool isMaximising,int cell,int alpha, int beta, int depth){
  gameState currState = returnGameState(b);
  int score;
  int childScore;
    
  if (depth == MAXIMUM_DEPTH){
    return evaluateBoard(b);
  }
  
  if (currState != TBD){
    int value = returnStateValue(currState);
    return value;
  }

  if (isBoardFull(*b) || allCellsWon(b)){
    return 0;
  }

  if (isCellFull(*b->cells[returnRow(cell)][returnColumn(cell)])){
    return minimaxFree(b,isMaximising,alpha,beta,depth);
  }

  int potentialPaths = 0;
  for (int j = 0; j < 9; j++){
  if (b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(j)][returnColumn(j)] == 0){
  potentialPaths++;
  }
  }

  pathArray *paths = initPathArray(potentialPaths);
  int pathCounter = 0;
  for (int x  = 0; x < 9; x++){
  if (b->cells[returnRow(cell)][returnColumn(cell)]->boxes[returnRow(x)][returnColumn(x)] == 0){
  if (isMaximising){
  AITestPlay(b,cell,x);
  } else {
  playerTestPlay(b,cell,x);
  }
  int score = evaluateCell(b,cell);
  paths->array[pathCounter]->score = score;
  paths->array[pathCounter]->position = x;
  pathCounter++;
  resetCell(b,cell,x);
  }
}

  sortPaths(paths,isMaximising);
  
  int halfOfPaths = paths->total / 2;
  if (isMaximising){
  score = A_SMALL_NUMBER;
  for (int i = 0; i  < halfOfPaths + 1; i++){
  int nextPos = paths->array[i]->position;
  AITestPlay(b,cell,nextPos);
  if (!cellWon(b,i) && !isCellFull(*b->cells[returnRow(i)][returnColumn(i)])){
	  childScore = minimaxRestricted(b,false,nextPos,alpha,beta,depth+1);
	} else {
	  childScore = minimaxFree(b,false,alpha,beta,depth+1);
	}
  score = MAX(score,childScore);
	alpha = MAX(alpha,score);
	resetCell(b,cell,nextPos);
	if (alpha <= beta){
	  break;
	}

}

  freePaths(paths);
  return score;

  } else {
    score = A_BIG_NUMBER;
    for (int i = 0; i < halfOfPaths; i++){
  int nextPos  = paths->array[i]->position;
  playerTestPlay(b,cell,nextPos);
  if (!cellWon(b,i) && !isCellFull(*b->cells[returnRow(i)][returnColumn(i)])){
  childScore = minimaxRestricted(b,true,nextPos,alpha,beta,depth+1);
} else {
  childScore = minimaxFree(b,true,alpha,beta,depth+1);
}
	score = MIN(score,childScore);
	beta = MIN(alpha,score);
	resetCell(b,cell,i);
	if (beta>= alpha){
	  break;
	}
    }
    freePaths(paths);
    return score;

}
}

//the AI works out the best move when it is a free move
void bestMoveFree(board *b, int *cellResult, int* positionResult){
  board *copy = initBoard();
  deepCopyBoard(copy,b);
  int minimaxScore = 0;
  int score = A_SMALL_NUMBER;
  for (int i = 0; i < 9; i++){
    if (!cellWon(copy,i)){
	for (int j = 0; j < 9; j++){
	  if (b->cells[returnRow(i)][returnColumn(i)]->boxes[returnRow(j)][returnColumn(j)] == 0){
	    AITestPlay(copy,i,j);
	    minimaxScore = minimaxRestricted(copy,false,i,A_SMALL_NUMBER,A_BIG_NUMBER,0);
	    if (score < minimaxScore){
	      score = minimaxScore;
	      *cellResult = i;
	      *positionResult= j;
	    }
	    resetCell(copy,i,j);}
	}
      }
      }
    freeBoard(copy);
}

//the AI works out the best move when it is a restricted move and stores that value in positionResult
void bestMoveRestricted(board *b, int *positionResult, int cell){
  board *copy = initBoard();
  deepCopyBoard(copy,b);
  int minimaxScore = 0;
  int score  = A_SMALL_NUMBER;
  for (int i = 0; i < 9; i++){
    if (b -> cells[returnRow(cell)][returnColumn(cell)] -> boxes[returnRow(i)][returnColumn(i)] == 0){
      AITestPlay(copy,cell,i);
      minimaxScore = minimaxRestricted(copy,false,i,A_SMALL_NUMBER,A_BIG_NUMBER,0);
	if (score <  minimaxScore){
	  score = minimaxScore;
	  *positionResult = i;
	}
	resetCell(copy,cell,i);
    }
  }


  freeBoard(copy);
}
//----------------------------------------------------------------------------------------------

int main(){
  bool firstTry = true;
  bool isPlayer = false;
  int *AIFreePickCell =  (int*) calloc(1,sizeof(int));
  int *AIFreePickPosition =  (int*) calloc(1,sizeof(int));
  int *AIRestPick =  (int*) calloc(1,sizeof(int));
  int *holdsNext = (int*) calloc(1,sizeof(int));
  FILECHECK(AIFreePickCell,"could not allocate integer for saving the AI's cell choice");
  FILECHECK(AIFreePickPosition,"could not allocate integer for saving the position that the AI chooses in free pick)");
  FILECHECK(AIRestPick,"could not allocate space for posiiton that AI chose in restricted pick");
  FILECHECK(holdsNext,"couldnt allocate space for holding the next cell position");
  board *b = initBoard();
  int cell;
  int position;
  char *response = (char*) calloc(4,sizeof(char));
  printf("Hello and welcome to ultimate tic tac toe. Would you like to go first? Please answer with y or n");
  printf("\n");
  fgets(response,2,stdin);
  isPlayer = strcmp("y",response) == 0;
  free(response);
  if (firstTry){
    if (isPlayer){
      printf("Welcome to ultimate tic tac toe. You make the first move. You will be X. Firstly pick the cell you would like to play in, numbered from 0 to 8 where 0 is the top left and 8 is the bottom right and then pick a  position inside the cell\n");
      scanf("%i",&cell);
      scanf("%i",&position);
      freePick(b,holdsNext,cell,position,isPlayer);
    } else {
      printf("The CPU will go first. It is O.\n.");
      bestMoveFree(b,AIFreePickCell,AIFreePickPosition);
      printf("The CPU has chosen to play in cell %i and position %i \n", *AIFreePickCell,*AIFreePickPosition);
      freePick(b,holdsNext,*AIFreePickCell,*AIFreePickPosition,isPlayer);
    }
    printf("\n\n\n\n\n\n");
    firstTry = false;
    isPlayer = !isPlayer;
    printBoard(b);
  }
  while (!gameWon(b) && !isBoardFull(*b)){
    if(isPlayer){
      if (!cellWon(b,*holdsNext) && !isCellFull(*b->cells[returnRow(*holdsNext)][returnColumn(*holdsNext)])){
	printf("You have to play in cell %i. Please pick a position\n",*holdsNext);
	scanf("%i",&position);
	restrictedPick(b,holdsNext,position,isPlayer);
      } else {
	printf("You have a free choice of cell to play in. Pick a cell and a position\n");
	scanf("%i",&cell);
	scanf("%i",&position);
	freePick(b,holdsNext,cell,position,isPlayer);
      }
    } else {
      printf("The CPU is thinking.....\n");
      printf("\n");
      sleep(2);
      if (!cellWon(b,*holdsNext) && !isCellFull(*b->cells[returnRow(*holdsNext)][returnColumn(*holdsNext)])){
	 printf("The CPU has to play in cell %i \n", *holdsNext);
	 bestMoveRestricted(b,AIRestPick,*holdsNext);
	 printf("The CPU has chosen to play in position %i\n", *AIRestPick);
	 restrictedPick(b,holdsNext,*AIRestPick,isPlayer);
      } else {
	printf("The CPU has a free pick\n");
	bestMoveFree(b,AIFreePickCell,AIFreePickPosition);
	printf("The CPU has chosen to play in cell %i and position %i \n", *AIFreePickCell,*AIFreePickPosition);
	freePick(b,holdsNext,*AIFreePickCell,*AIFreePickPosition,isPlayer);
      }
    }
    printf("\n\n\n\n\n");
    printBoard(b);
    isPlayer = !isPlayer;
  }
  if(!gameWon(b)){
    printf("Congratulations on a hard fought game that ended in a draw\n");
  } else {
    if (!isPlayer){
      printf("Congrats on winning.\n");
    } else {
      printf("Unlucky, the CPU won this time\n");
    }
  }

  freeBoard(b);
  free(AIFreePickCell);
  free(AIFreePickPosition);
  free(AIRestPick);
  free(holdsNext);
  return 0;
}
