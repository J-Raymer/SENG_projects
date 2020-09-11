#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#define MAX_LEN 40
#define MAX_LIMIT 200

void encode(char string[], int fileLen);
void decode(char string[], int fileLen);


int main (int argc, char *argv[]){
    
    char *fileName[MAX_LIMIT];
    char fileContents[MAX_LIMIT];
    int i = 0;

if (argc == 1){ // No parameters specified
   
      printf("Error: No input file specified!\n");
      exit(1);
   }

   if (argc == 2){ // Input file agrument specified, no flag argument
   
      // Check filename is in correct format
      FILE *fp = fopen(argv[1], "r");
      if (fp == NULL)
      {
         printf("Read error: file not found or cannot be read\n");
         exit(2);
      }
      fclose(fp);

      // Filename is in correct format, but we are still missing flag argumennt
      printf("Invalid Usage, expected: RLE {filename} [e | d]\n");
      exit(4);
   }

   if (argc == 3){ // Both arguments specified
   
      // Check flag is in proper format
      if (!(*argv[2] == 'e' || *argv[2] == 'd')){
      
         printf("Invalid Usage, expected: RLE {input_file} [e | d]\n");
         exit(4);
      }
   }

   if (argc > 3){ // Too manny arguments specified
   
      printf("Too many arguments, expected: RLE {input_file} [e | d]\n");
      exit(1);
   }

fileName[MAX_LIMIT] = argv[1];
FILE *fp = fopen(argv[1], "r");
      if (fp == NULL) // Checking again, just incase
      {
         printf("Read error: file not found or cannot be read\n");
         exit(2); // exit. Cannot open file pointers
      }

//store file contents in "fileContents" array
    while(!feof(fp)){
        fgets(fileContents, MAX_LIMIT, fp);
    }
    fclose(fp);
    int fileLen = strnlen(fileContents, MAX_LIMIT);
   
    for(i=0;i<fileLen;i++){
        char temp = fileContents[i];
        //check fileContents to make sure they are legal characters (1-9 , A-Z)
        if((temp >= 33 && temp <= 48) || (temp>= 58 && temp<= 64) || (temp >= 91 && temp <= 127)){
            printf("Error: Invalid format");
            exit(3);
       }

        //check if character is a space, if so make sure there are no subsequent characters other than more space
        if(temp == 32){
           int temp2 = fileContents[i+1];
           int j = i;
           for(j; j<fileLen; j++){
                if((temp2 >= 33 && temp2 <= 126 )){
                    printf("Error: Invalid format");
                    exit(3);
               }
           }
       }
    }   

   if (*argv[2] == 'e'){ // Encoding
   
       encode(fileContents, fileLen);
   }

   else { // Decoding
   
       decode(fileContents, fileLen);
   }

   return 0;
}

void encode(char string[], int fileLength){
    int i;
    int count = 1;

    // encodes string sent from file opened in main
    for(i=0;i<fileLength;i++){
        char temp = string[i];

        //check to see if there are numbers in string, if there are the string is not formated correctly for this application
        if(temp >= 48 && temp <= 57){
            printf("Error: String could not be encoded");
            exit(5);
        }
    }

    for(i=0;i<fileLength;i++){
        char temp = string[i];

        //increment count if characters repeat
        while(temp == string[i+1]){
            count++;
            i++;
        }

        //if characters do not repeat, print character
        if (count == 1){
            printf("%c", temp);
        }
        else {
            printf("%c%d", temp, count);
            count =1;
        }
    }
    exit (0);
}
void decode(char string[], int fileLength){
    int i;
    int j;
    int count;
    int numberBool = 0;
    int letterBool = 0;
    char test = string[0];
    int check = 0;
    int multiple;
    char tempChar;
    
    //test if first entry is a number, if it is then format is incorrect send error code -- quick check
    if (test >= 48 && test <= 57){
        printf("Error: String could not be decoded");
        exit (5);
    }

    //check formating #1
    for(i=0;i<fileLength;i++){
        char temp = string[i];

        //check to see if there are numbers in string
        if(temp >= 48 && temp <= 57){
            numberBool = 1;
        }     

        //check to see if there are letters in string
        if(temp >=65  && temp <= 90){
           letterBool = 1;
        }
    }

        //if there are are not both letters and numbers format is incorrect, send error code
        if((numberBool == 0 || letterBool == 0)){
        printf("Error: String could not be decoded");
        exit (5);
        }

    //check formatting #2
    for(i=0;i<fileLength;i++){
        check = (i % 2);
        char temp = string[i];

         //if i is even make sure it is not a number
        if (!check){
            if(temp >= 48 && temp <= 57){               
                printf("Error: String could not be decoded");
                exit (5);
            }
        }

        //if i is odd make sure it is not a letter
        if (check){
            if(temp >= 65 && temp <= 90){
                printf("Error: String could not be decoded");
                exit (5);
            }
        }
    }

    //decode and print
    for(i=0;i<fileLength;i++){
        check = (i % 2);
        char temp = string[i];
        if (!check){
            tempChar = temp;
            printf("%c",temp);
        }
        if (check){
            multiple = (int)temp;
            multiple -= 48;
            for(j=0;j<multiple-1;j++){
                printf("%c", tempChar);
            }   
        }
    }
    exit (0);
}