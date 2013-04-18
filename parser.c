/* AUTHOR: James Tanner */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "parser.h"
#include "mysh.h"

#define BUF_MAX_LEN 255
#define CMD_BUF_LEN 32

int expandBuffer(char**, int, int);
char* getHostName();
char* getHostNameShort();
char* getUserName();
char* getPathFromHome();

int parseCommand(const char* src, char*** argv)
{
   // Check if background task.
   int srclen = strlen(src);
   int background = NO;
   if (src[srclen-1] == '&'){
      background = YES;
      srclen--;
   }

   int buflen = CMD_BUF_LEN;
   int bufindex = 0;
   int cmdindex = 0;
   char** cmdbuf = (char**) emalloc(sizeof(char*) * buflen);

   int cmdlen = CMD_BUF_LEN;
   cmdbuf[bufindex] = (char*) emalloc (cmdlen);

   int srcindex = 0;
   char currchar;
   int newlen;
   int inQuote = NO;
   while (srcindex < srclen){
      // Get char.
      currchar = src[srcindex];

      // Expand command buffer, if needed.
      if (cmdindex >= cmdlen - 1){
         cmdlen += CMD_BUF_LEN;
         cmdbuf[bufindex] = (char*) erealloc(cmdbuf[bufindex], cmdlen);
      }

      // Move to next command, if needed.
      if (currchar == ' ' && !inQuote){
         if (cmdindex != 0){
            cmdbuf[bufindex][cmdindex] = '\0';
            bufindex++;
            cmdindex = 0;
            cmdlen = CMD_BUF_LEN;
            if (bufindex >= buflen - 1){
               buflen += CMD_BUF_LEN;
               cmdbuf = (char**) erealloc(cmdbuf, sizeof(char*) * buflen);
            }
            cmdbuf[bufindex] = (char*) emalloc (cmdlen);
         }
      }

      // Handle quotes
      else if (currchar == '\"'){
         if (inQuote == YES) inQuote = NO;
         else inQuote = YES;
      }

      // Write everything else.
      else {
         cmdbuf[bufindex][cmdindex] = currchar;
         cmdindex++;
      }

      // Continue.
      srcindex++;
   }

   // Add null terminators.
   if (cmdindex == 0){
      free(cmdbuf[bufindex]);
      cmdbuf[bufindex] = NULL;
   }
   else{
      cmdbuf[bufindex][cmdindex] = '\0';
      cmdbuf[bufindex + 1] = NULL;
   }

   // Return values.
   *argv = cmdbuf;
   return background;
}

void freeCmdbuf(char** cmdbuf)
{
   int index = 0;
   while (cmdbuf[index] != NULL)
      free(cmdbuf[index++]);

   free(cmdbuf);
}

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
   char* buf = (char*) emalloc(buflen);
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
               tempstr = getcwd(0,0);
            else
               tempstr = getPathFromHome();

            templen = strlen(tempstr);
            buflen = expandBuffer(&buf, buflen, templen);
            memcpy(&buf[writeindex], tempstr, templen);
            writeindex += templen - 1;
            currchar = tempstr[templen - 1];
            free(tempstr);
            break;

         case 'u': // Username
            tempstr = getUserName();
            templen = strlen(tempstr);
            buflen = expandBuffer(&buf, buflen, templen);
            memcpy(&buf[writeindex], tempstr, templen);
            writeindex += templen - 1;
            currchar = tempstr[templen - 1];
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
   char* newbuf = (char*) emalloc(newsize);
   memcpy(newbuf, *buf, origsize);
   free(*buf);
   *buf = newbuf;
   return newsize;
}

char* getHostName()
{
   char* hostname = (char*) emalloc(BUF_MAX_LEN + 1);
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

char* getUserName()
{
   char* username = getenv("USER");
   if (username == NULL)
      username = "";
   return username;
}

char* getPathFromHome()
{
   char* dir = getcwd(0,0);
   char* home = realpath(getenv("HOME"),NULL);
   int homelen = strlen(home);
   int dirlen = strlen(dir);

   if (dirlen >= homelen){
      int i = 0;
      while (i < homelen){
         if (home[i] != dir[i]){
            free(home);
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
   free(home);
   return dir;
}
