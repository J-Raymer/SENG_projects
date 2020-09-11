#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TRUE 1
#define FALSE 0

#define DICTSIZE 4096 /* allow 4096 entries in the dict  */
#define ENTRYSIZE 32

unsigned char dict[DICTSIZE][ENTRYSIZE]; /* of 30 chars max; the first byte */
                                         /* is string length; index 0xFFF   */
                                         /* will be reserved for padding    */
                                         /* the last byte (if necessary)    */

// These are provided below
int read12(FILE *infil);
int write12(FILE *outfil, int int12);
void strip_lzw_ext(char *fname);
void flush12(FILE *outfil);

void encode(FILE *in, FILE *out);
void decode(FILE *in, FILE *out);

int main(int argc, char *argv[])
{
   if (argc == 1) // No parameters specified
   {
      printf("Error: No input file specified!\n");
      exit(1);
   }

   if (argc == 2) // Input file agrument specified, no flag argument
   {

      // Check filename is in correct format
      FILE *fp = fopen(argv[1], "r");
      if (fp == NULL)
      {
         printf("Read error: file not found or cannot be read\n");
         exit(2);
      }
      fclose(fp);

      // Filename is in correct format, but we are still missing flag argumennt
      printf("Invalid Usage, expected: LZW {input_file} [e | d]\n");
      exit(4);
   }

   if (argc == 3) // Both arguments specified
   {

      // Check flag is in proper format
      if (!(*argv[2] == 'e' || *argv[2] == 'd'))
      {
         printf("Invalid Usage, expected: LZW {input_file} [e | d]\n");
         exit(4);
      }
   }

   if (argc > 3) // Too manny arguments specified - Not in assignment scope... but added it anyways
   {
      printf("Too many arguments, expected: LZW {input_file} [e | d]\n");
      exit(1);
   }

   if (*argv[2] == 'e') // Encoding
   {
      char outputFileName[strlen(argv[1]) + 4]; // Add 4 for string ".LZW"
      strcpy(outputFileName, argv[1]);
      strcat(outputFileName, ".LZW");

      FILE *ifp = fopen(argv[1], "rb");
      FILE *ofp = fopen(outputFileName, "wb");

      if (ifp == NULL || ofp == NULL) // Checking again, just incase
      {
         printf("Read error: file not found or cannot be read\n");
         exit(2); // exit. Cannot open file pointers
      }

      encode(ifp, ofp);

      fclose(ifp);
      fclose(ofp);
   }
   else // Decoding
   {
      char outputFileName[strlen(argv[1])];
      strcpy(outputFileName, argv[1]);
      strip_lzw_ext(outputFileName);

      FILE *ifp = fopen(argv[1], "rb");
      FILE *ofp = fopen(outputFileName, "wb");

      if (ifp == NULL || ofp == NULL) // Checking again, just incase
      {
         printf("Read error: file not found or cannot be read\n");
         exit(2); // exit. Cannot open file pointers
      }

      decode(ifp, ofp);

      fclose(ifp);
      fclose(ofp);
   }

   return 0;
}

int getLength(unsigned char c)
{
   return (int)(c - '0');
}

char setLength(int i)
{
   return (char)(i + '0');
}

// Create a string in our special format
char *createString(unsigned char str[31], int size)
{
   char *myStr = malloc(32);
   myStr[0] = setLength(size);
   int i;
   for (i = 1; i <= size; i++)
   {
      myStr[i] = str[i - 1];
   }

   return myStr;
}

char *createCharString(unsigned char c)
{
   char *myStr = malloc(32);
   myStr[0] = setLength(1);
   myStr[1] = c;
   return myStr;
}

// Append character to first string (str)
void appendChar(unsigned char *str, unsigned char c)
{
   int len = getLength(str[0]);

   if ((len + 1) < 32)
   {
      str[0] = setLength(len + 1);
      str[len + 1] = c;
   }
}

// Replace first string (str1) with second string (str2)
void strReplace(unsigned char *str1, unsigned char *str2)
{
   int len = getLength(str2[0]);
   int i;
   for (i = 0; i <= len; i++)
   {
      str1[i] = str2[i];
   }
}

// Append string to first string (str1)
void appendString(unsigned char *str1, unsigned char *str2)
{

   int len1 = getLength(str1[0]);
   int len2 = getLength(str2[0]);

   int megaLen = (len1 + len2);
   if (megaLen < 32)
   {
      int i;
      int j;

      for (i = len1 + 1, j = 1; i <= megaLen; i++, j++)
      {
         str1[i] = str2[j];
      }

      str1[0] = setLength(megaLen);
   }
}

// Converts ascii value to a single character string
char *asciiToStr(int ascii)
{
   char *str = malloc(2);
   str[0] = ascii;
   str[1] = '\0';
   return str;
}

// Converts char to string
char *charToStr(char c)
{
   char *str = malloc(2);
   str[0] = c;
   str[1] = '\0';
   return str;
}

int dynamic_index = 256;
bool isDictFull()
{
   return dynamic_index >= 4095;
}

void addStrToDict(unsigned char dict[DICTSIZE][ENTRYSIZE], unsigned char *str)
{
   int len = getLength(str[0]);
   if (len == 1)
   {
      unsigned char c = str[0];
      dict[(int)c][0] = setLength(1);
      dict[(int)c][1] = c;
   }
   if (len > 1 && len <= 31 && !isDictFull())
   {
      int i;
      for (i = 0; i <= len; i++) // Write rest of string into dict[code][1] to dict[code][str_length]
      {
         dict[dynamic_index][i] = str[i];
      }
      dynamic_index++;
   }
}

// Checks if a code exists in the dictionary
bool isInDict(unsigned char dict[DICTSIZE][ENTRYSIZE], unsigned char *str)
{
   int len = getLength(str[0]);
   if (len == 0) // Not a string that we would have added
   {
      return false;
   }
   if (len == 1) // Ascii character, already aded, but double check dict[i][0] is set
   {
      return true;
   }
   if (len > 1 && len <= 31)
   {
      int i;
      for (i = 256; i <= 4095; i++)
      {
         bool valid = true;
         if (len == getLength(dict[i][0])) // only check strings where the length is the same, for speed
         {
            int j;
            for (j = 1; j <= len && valid; j++) // check string in dict to one passed in: str
            {
               if (dict[i][j] != str[j])
               {
                  valid = false;
               }
            }
            if (valid)
            {
               return true;
            }
         }
      }
   }
   return false; // We didn't find str in dictionary, or str length was out of bounds
}

// Gets a code from the dictionary
int getCodeFromDict(unsigned char dict[DICTSIZE][ENTRYSIZE], unsigned char *str)
{
   int len = getLength(str[0]);
   if (len == 1)
   {
      unsigned char c = str[1];
      return (int)c;
   }
   if (len > 1 && len <= 31)
   {
      int i;
      for (i = 256; i <= 4095; i++)
      {
         bool valid = true;
         if (len == getLength(dict[i][0])) // only check strings where the length is the same, for speed
         {
            int j;
            for (j = 1; j <= len && valid; j++) // check string in dict to one passed in: str
            {
               if (dict[i][j] != str[j])
               {
                  valid = false;
               }
            }
            if (valid)
            {
               return i;
            }
         }
      }
   }

   printf("NOT IN DICTIONARY ERROR");
   exit(EXIT_FAILURE); // not in dictionary
}

// write dictionary string (thats at a certain index) to a file
void writeDictToFile(unsigned char dict[DICTSIZE][ENTRYSIZE], FILE *out, int index)
{
   int len = getLength(dict[index][0]);
   int i;
   for (i = 1; i <= len; i++)
   {
      fputc(dict[index][i], out);
   }
}

// write a string to a file
void writeStrToFile(char *str, FILE *out)
{
   int len = getLength(str[0]);
   int i;
   for (i = 1; i <= len; i++)
   {
      fputc(str[i], out);
   }
}

// Checks if a code is in the dictionary
bool isCodeInDict(unsigned char dict[DICTSIZE][ENTRYSIZE], int code)
{
   if (code < 0 || code > 4095)
   {
      return false; // out of array bounds
   }

   if (code <= 255) // ascii
   {
      return true;
   }
   else // dynamic, check if first index is set
   {
      return getLength(dict[code][0]) >= 0;
   }
}

/*****************************************************************************/
/* encode() performs the Lempel Ziv Welch compression from the algorithm in  */
/* the assignment specification. The strings in the dictionary have to be    */
/* handled carefully since 0 may be a valid character in a string (we can't  */
/* use the standard C string handling functions, since they will interpret   */
/* the 0 as the end of string marker). Again, writing the codes is handled   */
/* by a separate function, just so I don't have to worry about writing 12    */
/* bit numbers inside this algorithm.                                        */
void encode(FILE *in, FILE *out)
{
   unsigned char dict[DICTSIZE][ENTRYSIZE];
   int i;
   for (i = 0; i <= 255; i++)
   {
      dict[i][0] = setLength(1);
      dict[i][1] = (unsigned char)i;
   }
   dict[4095][0] = setLength(1);
   dict[4095][1] = (unsigned char)0;

   int k; // declared as int and not char for EOF character
   unsigned char *w = createString("", 0);

   while (k != EOF)
   {
      k = fgetc(in); // get k from file

      if (k == EOF) // so we dont do an extra loop
      {
         break;
      }

      unsigned char *k_char = createCharString(k); // convert interger k to our string format
      unsigned char *wk = createString("", 0);

      appendString(wk, w);
      appendString(wk, k_char);

      if (isInDict(dict, wk)) // Check if wk is in dictionary
      {
         // set w to wk
         strReplace(w, wk);
      }
      else
      {
         
            int code = getCodeFromDict(dict, w); // get code for w
            write12(out, code);

            // add wk to the dict (if its not full)
            if (dynamic_index >= DICTSIZE){
            addStrToDict(dict, wk);
            }
            // set w to k
            strReplace(w, k_char);
         }
      
   }

   // output for code w
   int code = getCodeFromDict(dict, w);
   write12(out, code);

   flush12(out);
}

/*****************************************************************************/
/* decode() performs the Lempel Ziv Welch decompression from the algorithm   */
/* in the assignment specification.                                          */
void decode(FILE *in, FILE *out)
{
   unsigned char dict[DICTSIZE][ENTRYSIZE];
   int i;
   for (i = 0; i <= 255; i++)
   {
      dict[i][0] = setLength(1);
      dict[i][1] = (unsigned char)i;
   }
   dict[4095][0] = setLength(1);
   dict[4095][1] = (unsigned char)0;

   // Read a code k from the encoded file
   int k = read12(in);

   // Output dict[k]
   writeDictToFile(dict, out, k);

   // Set w to dict[k]
   unsigned char *w = createString("", 0);
   appendString(w, dict[k]);
 
   while (k != EOF)
   {
      k = read12(in);            // get k from file
      if (k == EOF || k == 4095) // if we read end of file or extra padding
      {
         break;
      }

      // if k is in the dictionary
      if (isCodeInDict(dict, k))
      {
         // output dict[k]
         writeDictToFile(dict, out, k);

         // add w + first character of dict[k] to the dict
         unsigned char *temp = createString("", 0);

         appendString(temp, w);
         appendChar(temp, dict[k][1]);
         addStrToDict(dict, temp);
      }
      else
      {
         // add w + first char of w to the dict and output it
         unsigned char *temp2 = createString("", 0);
         appendString(temp2, w);
         appendChar(temp2, w[1]);
         addStrToDict(dict, temp2);
         writeStrToFile(temp2, out);
      }

      // set w to dict[k]
      strReplace(w, dict[k]);
      i++;
   }
}

/*****************************************************************************/
/* read12() handles the complexities of reading 12 bit numbers from a file.  */
/* It is the simple counterpart of write12(). Like write12(), read12() uses  */
/* static variables. The function reads two 12 bit numbers at a time, but    */
/* only returns one of them. It stores the second in a static variable to be */
/* returned the next time read12() is called.                                */
int read12(FILE *infil)
{
   static int number1 = -1, number2 = -1;
   unsigned char hi8, lo4hi4, lo8;
   int retval;

   if (number2 != -1)   /* there is a stored number from   */
   {                    /* last call to read12() so just   */
      retval = number2; /* return the number without doing */
      number2 = -1;     /* any reading                     */
   }
   else /* if there is no number stored    */
   {
      if (fread(&hi8, 1, 1, infil) != 1) /* read three bytes (2 12 bit nums)*/
         return (-1);
      if (fread(&lo4hi4, 1, 1, infil) != 1)
         return (-1);
      if (fread(&lo8, 1, 1, infil) != 1)
         return (-1);

      number1 = hi8 * 0x10;                /* move hi8 4 bits left            */
      number1 = number1 + (lo4hi4 / 0x10); /* add hi 4 bits of middle byte    */

      number2 = (lo4hi4 % 0x10) * 0x0100; /* move lo 4 bits of middle byte   */
                                          /* 8 bits to the left              */
      number2 = number2 + lo8;            /* add lo byte                     */

      retval = number1;
   }

   return (retval);
}

/*****************************************************************************/
/* write12() handles the complexities of writing 12 bit numbers to file so I */
/* don't have to mess up the LZW algorithm. It uses "static" variables. In a */
/* C function, if a variable is declared static, it remembers its value from */
/* one call to the next. You could use global variables to do the same thing */
/* but it wouldn't be quite as clean. Here's how the function works: it has  */
/* two static integers: number1 and number2 which are set to -1 if they do   */
/* not contain a number waiting to be written. When the function is called   */
/* with an integer to write, if there are no numbers already waiting to be   */
/* written, it simply stores the number in number1 and returns. If there is  */
/* a number waiting to be written, the function writes out the number that   */
/* is waiting and the new number as two 12 bit numbers (3 bytes total).      */
int write12(FILE *outfil, int int12)
{
   static int number1 = -1, number2 = -1;
   unsigned char hi8, lo4hi4, lo8;
   unsigned long bignum;

   if (number1 == -1) /* no numbers waiting             */
   {
      number1 = int12; /* save the number for next time  */
      return (0);      /* actually wrote 0 bytes         */
   }

   if (int12 == -1)     /* flush the last number and put  */
      number2 = 0x0FFF; /* padding at end                 */
   else
      number2 = int12;

   bignum = number1 * 0x1000; /* move number1 12 bits left      */
   bignum = bignum + number2; /* put number2 in lower 12 bits   */

   hi8 = (unsigned char)(bignum / 0x10000);               /* bits 16-23 */
   lo4hi4 = (unsigned char)((bignum % 0x10000) / 0x0100); /* bits  8-15 */
   lo8 = (unsigned char)(bignum % 0x0100);                /* bits  0-7  */

   fwrite(&hi8, 1, 1, outfil); /* write the bytes one at a time  */
   fwrite(&lo4hi4, 1, 1, outfil);
   fwrite(&lo8, 1, 1, outfil);

   number1 = -1; /* no bytes waiting any more      */
   number2 = -1;

   return (3); /* wrote 3 bytes                  */
}

/** Write out the remaining partial codes */
void flush12(FILE *outfil)
{
   write12(outfil, -1); /* -1 tells write12() to write    */
} /* the number in waiting          */

/** Remove the ".LZW" extension from a filename */
void strip_lzw_ext(char *fname)
{
   char *end = fname + strlen(fname);

   while (end > fname && *end != '.' && *end != '\\' && *end != '/')
   {
      --end;
   }
   if ((end > fname && *end == '.') &&
       (*(end - 1) != '\\' && *(end - 1) != '/'))
   {
      *end = '\0';
   }
}
