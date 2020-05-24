#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>


struct registerRep{
  int value;
  int [32]binary; // i made it this way because it will be easier to do all this with actual integers - there is a method that will take an int and a int array and write the binary representation in that array
};

void initliaseRegisters(struct registerRep *location){
  for (int i = 0; i < 15; ++i){
    registerRep registerNew;
    location[i] = registerNew;
  }
  initliaseReigstersToZero(location);
}

void initlaiseRegistersToZero(struct registerRep *location){
  for (int i = 0; i < 15; ++i){
    registerRep curr = location[i];
    curr.value = 0;
    setBinarytoZero(curr.binary);
}


void setBinarytoZero(int array*){
  for (int i = 0; i < 32; ++i){
    array[i] = 0;
  }
}

void printRegisters(struct registerRep *array){
  for (int i = 0; i < 15; ++i){
    printRegister(array[i]);
    if (i != 14){
      printf("\n");
    }
  }
}
  

void printRegister(struct registerRep r){
  for (int i = 31; i > -1; --i){
    printf("%d",r.binary[i]);
  }
}
  

void setBinaryArray(int *array, int value){
   int mask = 1;
   for (int i = 31; i > -1; --i){
     if ((value & mask) == 0){
       array[i] = 0;
     } else {
       array[i] = 1;
     }
   }
 }

 void setRegister(struct registerRep r, int value){
   r.value = value;
   setBinaryArray(r.binary,value);
 }

int main(int argc, char **argv) {

  FILE *binaryFile;
  if (( binaryFile = fopen(argv[1],"rb")== NULL){
      printf("error file not found");
      exit(1);
    }//this will let us read the binary file

    int *memory = (int*)  calloc(65536,1); // this is me trying to initilaise the memory part. just hving 2^16 bits. Not really sure what I'm doing here
    struct registerRep *registers = (struct registerRep*) calloc(15,sizeof(struct registerRep)); // creates the registers
    initlaiseRegisters(registers);
    printRegisters(registers);
    setRegister(registers,19);
  printRegisters(registers):
  
  return EXIT_SUCCESS;
}
