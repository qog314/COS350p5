/* AUTHOR: James Tanner */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "parseEscapes.h"
#include "mysh.h"

#define BUF_MAX_LEN 255

int expandBuffer(char**, int, int);
char* getHostName();
char* getHostNameShort();
char* getPathFromHome();

/* parseEscapes:
 * This function takes a string and parses escaped characters.
 * So, a string including the character sequence '\\','n'
 * would be converted to include '\n'.
 *
 * Returns: A newly allocated string, containing the parsed
 * characters.
 *
 * Note: This should be freed to avoid memory leaks.*/
char* parseEscapes(char *string)
{
   int buflen = strlen(string) + 1;
   char* buf = malloc(buflen);
   if (buf == NULL)
      fatal("Malloc failed", "parseEscapes:buf", errno);

   int readindex = 0;
   int writeindex = 0;
   int scanned;
   char currchar;
   unsigned int val;
   char* tempstr;
   char* tempbuf;
   int templen;

   // Iterate through all characters in the string.
   while ((currchar = string[readindex]) != 0){

      // Handle escaped characters.
      if (currchar == '\\'){
         readindex++;
         currchar = string[readindex];
         switch (currchar){
         case 0:
            currchar = '\\';
            break;
         case 'r':
            currchar = '\r';
            break;
         case 'n':
            currchar = '\n';
            break;
         case 't':
            currchar = '\t';
            break;
         case 'v':
            currchar = '\v';
            break;
         case 'a':
            currchar = '\a';
            break;

         case '0': // Octal:
            sscanf(&string[readindex],"%o",&val);
            readindex++;
            if (val > 07) readindex++;
            if (val > 077) readindex++;
            currchar = (char)val;
            break;

         case 'x':
            // WARNING: Not properly tested
            scanned = sscanf(&string[readindex],"%x",&val);
            readindex += scanned;
            currchar = (char)val;
            break;

         case 'h': // Hostname
         case 'H':
            if (currchar == 'H')
               tempstr = getHostName();
            else
               tempstr = getHostNameShort();

            templen = strlen(tempstr);

            buflen = expandBuffer(&buf, buflen, templen);
            memcpy(&buf[writeindex], tempstr, templen);
            writeindex += templen - 1;
            currchar = tempstr[templen - 1];
            free(tempstr);
            break;

         case 'w': // Working directory
         case 'W':
            if (currchar == 'W')
               tempstr = get_current_dir_name();
            else
               tempstr = getPathFromHome();

            templen = strlen(tempstr);
            buflen = expandBuffer(&buf, buflen, templen);
            memcpy(&buf[writeindex], tempstr, templen);
            writeindex += templen - 1;
            currchar = tempstr[templen - 1];
            free(tempstr);
            break;
         }
      }

      // Write character and continue.
      buf[writeindex] = currchar;
      writeindex++;
      readindex++;
   }

   // Terminate string.
   buf[writeindex] = 0;
   return buf;
}

int expandBuffer(char** buf, int origsize, int increase){
   int newsize = origsize + increase;
   char* newbuf = malloc(newsize);
   if (newbuf == NULL)
      fatal("Malloc failed", "expandBuffer", errno);
   memcpy(newbuf, *buf, origsize);
   free(*buf);
   *buf = newbuf;
   return newsize;
}

char* getHostName()
{
   char* hostname = malloc(BUF_MAX_LEN + 1);
   if (hostname == NULL)
      fatal("Malloc failed", "getHostName", errno);
   gethostname(hostname, BUF_MAX_LEN);
   hostname[BUF_MAX_LEN] = 0;
   return hostname;
}

char* getHostNameShort()
{
   char* hostname = getHostName();
   int i = 0;
   char currchar;
   do{
      i++;
      currchar = hostname[i];
   }while (currchar != 0 && currchar != '.');
   hostname[i] = 0;
   return hostname;
}

char* getPathFromHome()
{
   char* dir = get_current_dir_name();
   char* home = getenv("HOME");
   int homelen = strlen(home);
   int dirlen = strlen(dir);

   if (dirlen >= homelen){
      int i = 0;
      while (i < homelen){
         if (home[i] != dir[i]){
            return dir;
         }
         i++;
      }

      // Found relative path.
      dir[0] = '~';
      int remaining = dirlen - i;
      memmove(&dir[1],&dir[i],remaining);
      dir[remaining + 1] = 0;
   }

   return dir;
}
