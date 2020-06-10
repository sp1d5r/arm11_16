#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int sum(int a, int b){
  return a+b;
}

void createStringArray(char **array, int length, int maxSize)
{
    for (int i = 0; i < length; i++)
    {
        array[i] = (char *)calloc(maxSize, sizeof(char));
    }
}

void replaceEqualsWithHashtag(char* str1, char* str2){
    str1[0] = '#';

    for(int i=1; i<strlen(str2); i++){
        str1[i] = str2[i];
    }
}

int main(void){
    char **ldrInstructions = (char **)calloc(3, sizeof(char *));
    createStringArray(ldrInstructions, 3, 10);
    ldrInstructions[0] = "ldr";
    ldrInstructions[1] = "r0";
    ldrInstructions[2] = "=0x02";

    printf("%s \n", ldrInstructions[2]);

    // not remove equals sign and replace it with a hashtag
    printf("%lu \n", strlen(ldrInstructions[2]));
    char new_string[strlen(ldrInstructions[2])];
    replaceEqualsWithHashtag(new_string,ldrInstructions[2]);
    printf("%s \n", new_string);
}
