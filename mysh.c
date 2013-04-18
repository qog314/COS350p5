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
#include "parser.h"

#define  DFL_PROMPT "\\033[1;33m\\u@\\h\\033[0m:\\033[1;34m\\w\\033[0m\\$ "
#define  FORCE_DFL_PROMPT 0
#define  DISPLAY_WARN 1
#define  WARN_MSG "[\\033[1;31mMYSH\\033[0m] "

int main()
{
   char	*cmdline, *prompt, *promptStr, **arglist;
   int	result, background;
   void	setup();

   char* env  = getenv("PS1");
   if (FORCE_DFL_PROMPT || env == NULL)
   {
      promptStr = (char*) emalloc(strlen(DFL_PROMPT) + 1);
      strcpy(promptStr, DFL_PROMPT);
   }
   else {
      int envlen = strlen(env);
      promptStr = (char*) emalloc(envlen + 1);
      memcpy(promptStr, env, envlen + 1);
   }

   // Add warning flag, to prevent confusion.
   if (DISPLAY_WARN){
      int warnlen = strlen(WARN_MSG);
      int promptlen = strlen(promptStr);
      char* temp = (char*) emalloc(warnlen + promptlen + 1);
      temp[0] = '\0';
      strcat(temp, WARN_MSG);
      strcat(temp, promptStr);
      free(promptStr);
      promptStr = temp;
   }

   // Parse prompt the first time.
   prompt = parseEscapes(promptStr);

   setup();

   while ( (cmdline = next_cmd(prompt, stdin)) != NULL ){
      background = parseCommand(cmdline, &arglist);

// TODO Remove
//printf("arglist: ");
//int i = 0;
//while (arglist[i] != NULL){
//   printf("%s, ", arglist[i]);
//   i++;
//}
//printf("\\0\n");

      result = execute(arglist);
      freeCmdbuf(arglist);
      free(cmdline);

      // Reparse the prompt, for escape characters such as \h.
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
   signal(SIGQUIT, handleSigChild);
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

