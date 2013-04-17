/**  smsh1.c  small-shell version 1
 **		first really useful version after prompting shell
 **		this one parses the command line into strings
 **		uses fork, exec, wait, and ignores signals
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "mysh.h"

#define	DFL_PROMPT	"Command > "

int main()
{
   char	*cmdline, *prompt, **arglist;
   int	result;
   void	setup();

   prompt = DFL_PROMPT;
   setup();

   while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
      if ( (arglist = splitline(cmdline)) != NULL  ){
         result = execute(arglist);
         freelist(arglist);
      }
      free(cmdline);
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

