#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>


struct registerRep{
  int value;
  int binary[32];
};


void  initialiseRegisters(struct registerRep *registerArray){
  for (int i = 0; i < 15; i++){
    setRegister(registerArray[i],0);
  }
}


void printRegisters(struct registerRep *array){
  for (int i = 0; i < 15; i++){
    printRegister(array[i]);
    printf("\n");
  }
}
  

void printRegister(struct registerRep r){
  for (int  i = 0; i < 32; i++){
    printf("%i",r.binary[i]);
  }
}


void setRegister(struct registerRep *r, int value){
   r->value = value;
   setBinaryArray(r->binary,value);
 }
  

void setBinaryArray(int *array, int value){
   int mask = 1;
   for (int i = 31; i > -1; i--){
     if ((value & mask) == 0){
       array[i] = 0;
     } else {
       array[i] = 1;
     }
     mask = mask << 1;
   }
 }

void setBinaryBit(struct registerRep *r, int position, int setTo){
  assert(setTo == 1 || setTo == 0);
  r->binary[position] = setTo;
  
}

void updateRegisterFromBinArray(struct registerRep *r){
  int total = 0;
  for (int i = 31; i > -1; i--){
    int power = (int) pow(2.0, (double) i - 31); //this will just calculate what power of 2 we need to use
    total = total + (power * r->binary[i]);
  }
  r->value = total;
}
  
  
  

void finish(){
  printf("\n");
}

int *createInstruction(){
  return (int*) calloc(32,sizeof(int));
}



int main(int argc, char **argv) {

  int instructionCout = 0;
  size_t instructionSize = 32*sizeof(int); 
  FILE *binaryFile;
  if ((binaryFile = fopen(argv[1],"rb"))== NULL){
      printf("error file not found");
      exit(1);
    }
 
  int buffer[32];

  size_t read;
  int **instructions = (int**) calloc(1,sizeof(int*)); 
  do {
    read = fread(buffer,sizeof(int),32,binaryFile);
    instructions = (int**) realloc(instructions,(instructionCout + 2) * sizeof(int)); // add a new 32 integer array to the ting I'm tired.
    int *instructionAddress = createInstruction();
    instructionAddress = buffer;
    instructions[instructionCout] = instructionAddress;
    instructionCout = instructionCout + 1;
     
  } while (read  > 0);

  fclose(binaryFile);

    int *memory = (int*)  calloc(65536,1); // this is me trying to initilaise the memory part. just hving 2^16 bits. Not really sure what I'm doing here
    struct registerRep *registers = (struct registerRep*) calloc(15,sizeof(struct registerRep));
    printRegisters(registers);
    setRegister(&registers[2],15);
    finish();
    printRegisters(registers);
  return EXIT_SUCCESS;
}
