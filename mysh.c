/**  smsh1.c  small-shell version 1
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include "mysh.h"
#include "parseEscapes.h"

#define  DFL_PROMPT "\\033[1;34mShell\\033[0m \\h > "

int main()
{
   char	*cmdline, *prompt, **arglist;
   int	result;
   void	setup();

   char* promptStr  = getenv("PS1");
   if (promptStr == NULL)
   {
      promptStr = malloc(strlen(DFL_PROMPT) + 1);
      strcpy(promptStr, DFL_PROMPT);
   }
   // TODO Add warning flag, to prevent confusion.
   prompt = parseEscapes(promptStr);

   setup();

   while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
      if ( (arglist = splitline(cmdline)) != NULL  ){
         result = execute(arglist);
         freelist(arglist);
      }
      free(cmdline);

      // Recalculate the prompt, for escape characters.
      prompt = parseEscapes(promptStr);
   }
   return 0;
}

/*
 * purpose: initialize shell
 * returns: nothing.
 */
void setup()
{
   signal(SIGINT, SIG_IGN);
   signal(SIGQUIT, SIG_IGN);
}


/*
 * purpose: exits the shell with the given exit code.
 * returns: nothing.
 */
void exitShell (int status)
{
   // TODO Kill all sub-threads to avoid zombies.
   exit(status);
}

void fatal(char *s1, char *s2, int n)
{
   fprintf(stderr,"Error: %s,%s\n", s1, s2);
   exit(n);
}

