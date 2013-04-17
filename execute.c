/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define EXIT "exit"

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, or -1 on error
 *  errors: -1 on fork() or wait() errors
 */
{
   int	pid ;
   int	child_info = -1;

   if ( argv[0] == NULL )		/* nothing succeeds	*/
      return 0;

   // Build in commands:
   if (strcmp(argv[0], EXIT) == 0){
      if (argv[1] == NULL)
         exitShell(0);
      else
         exitShell(atoi(argv[1]));
   }


   if ( (pid = fork())  == -1 )
      perror("fork");
   else if ( pid == 0 ){
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      execvp(argv[0], argv);
      perror("cannot execute command");
      exit(1);
   }
   else {
      if ( waitpid(pid, &child_info, 0) == -1 )
         perror("wait");
   }
   return child_info;
}
