#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>


struct registerRep{
  int value;
  int binary[32];
};

void initialiseRegisters(struct registerRep *registerArray);
void printRegisters(struct registerRep *array);
void printRegister(struct registerRep r);
void setRegister(struct registerRep *r, int value);
void updateRegisterFromBinArray(struct registerRep *r);
int *createInstruction();
void setBinaryBit(struct registerRep *r, int position, int setTo);
void flipAllBits(int *array);
void addOne(int *array);
void setBinaryArray(int *array, int value);
void setBinaryArray8Bit(int *array, int value);

void  initialiseRegisters(struct registerRep *registerArray){
  for (int i = 0; i < 15; i++){
    setRegister(&registerArray[i],0);
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

void updateRegisterFromBinArray(struct registerRep *r){
  int total = 0;
  for (int i = 31; i > -1; i--){
    int power = (int) pow(2.0, (double) i - 31); //this will just calculate what power of 2 we need to use
    total = total + (power * r->binary[i]);
  }
  r->value = total;
}

int *createInstruction(){
  return (int*) calloc(32,sizeof(int));
}

void setBinaryBit(struct registerRep *r, int position, int setTo){
  assert(setTo == 1 || setTo == 0);
  r->binary[position] = setTo;
  updateRegisterFromBinArray(r);
}

struct registerRep *createRegisters(){
  return (struct registerRep*) calloc(15,sizeof(struct registerRep));
}


void flipAllBits(int *array){
  for (int i = 0; i < 8; i++){
    if (array[i] == 1){
      array[i] = 0;
    } else {
      array[i] = 1;
    }
  }
}

void addOne(int *array){
  int carry = 0;
  int i = 7;
  do{
    if(array[i]== 1){
      array[i] = 0;
      carry = 1;
    } else {
      array[i] = 1;
      carry = 0;
    }
    i--;
  }while (carry);
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


void setBinaryArray8Bit(int *array, int value){
   int mask = 1;
   for (int i = 8; i > -1; i--){
     if ((value & mask) == 0){
       array[i] = 0;
     } else {
       array[i] = 1;
     }
     mask = mask << 1;
   }
 }



void bufferSetZero(int *buffer){
  for (int i = 0; i < 8; i++){
    buffer[i] = 0;
  }
}

void bigEndianConvert(int* array){
  for (int i = 0; i < 16; i++){
    int begin = array[i];
    int end = array[31 - i];
    array[i] = end;
    array[31 - i] = begin;
  }
}

void twosCompConvert(int i, int *array){
  setBinaryArray8Bit(array,abs(i));
  if (i < 0){
    flipAllBits(array);
    addOne(array);
  }
}

int *combineToOne(int *one, int *two, int *three, int *four, int *array){
   for (int i = 0; i < 8; i++){
     array[i] = one[i];
     array[i+8] = two[i];
     array[i+16] = three[i];
     array[i+24] = four[i];
   }
   bigEndianConvert(array);
   return array;
 }
  
int *intsToBinArray(int a, int b, int c, int d, int *array){
   int firstByte[8];
   int secondByte[8];
   int thirdByte[8];
   int fourthByte[8];
   twosCompConvert(a,firstByte);
   twosCompConvert(b,secondByte);
   twosCompConvert(c,thirdByte);
   twosCompConvert(d,fourthByte);
   combineToOne(firstByte,secondByte,thirdByte,fourthByte,array);
   return array;
}

int fileSize(FILE *file){
  int size = 0;
  fseek(file,0,SEEK_END);
  size = ftell(file);
  fseek(file,0,SEEK_SET);
  return size;
}

int main(int argc, char **argv) {

  // int instructionCout = 0;
  // size_t instructionSize = 32*sizeof(int); 
  FILE *binaryFile = binaryFile = fopen(argv[1],"rb");
  if (binaryFile == NULL){
      printf("error file not found\n");
      exit(1);
    }
  int fileLength = fileSize(binaryFile);
  int a[4];
 
  char *buffer = (char*) calloc(1,fileLength+1);
  fread(buffer,fileLength,1,binaryFile);
  fclose(binaryFile);
  for (int i = 0; i < 4; i++){
    a[i] = buffer[i];
  }
  int array[32];
  intsToBinArray(a[0],a[1],a[2],a[3],array);
  

//  int *memory = (int*)  calloc(65536,1); // this is me trying to initilaise the memory part. just hving 2^16 bits. Not really sure what I'm doing here
  struct registerRep *registers = createRegisters();
  printRegisters(registers);
   
  return EXIT_SUCCESS;
}

// /home/ayoob/Programming/C/arm11_16/testing/arm11_testsuite/test_cases/add01

