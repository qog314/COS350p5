/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define EXIT "exit"
#define CD "cd"
void changedir(char*);

int execute(char *argv[])
/*
 * purpose: run a program passing it arguments
 * returns: status returned via wait, -1 on error,
 * or 0 on directory change.
 *  errors: -1 on fork() or wait() errors.
 */
{
   int	pid ;
   int	child_info = -1;

   if ( argv[0] == NULL )		/* nothing succeeds	*/
      return 0;

   // Build in commands:
   if (strcmp(argv[0], EXIT) == 0){
      if (argv[1] == NULL) exitShell(0);
      else exitShell(atoi(argv[1]));
   }

   if  (strcmp(argv[0], CD) == 0){
      if (argv[1] == NULL) changedir("~");
      else changedir(argv[1]);
      return 0;
   }


   // Execute external command.
   if ( (pid = fork())  == -1 )
      perror("fork");
   else if ( pid == 0 ){
      signal(SIGINT, SIG_DFL);
      signal(SIGQUIT, SIG_DFL);
      execvp(argv[0], argv);
      perror("cannot execute command");
      return -1;
   }
   else {
      if ( waitpid(pid, &child_info, 0) == -1 )
         perror("wait");
   }
   return child_info;
}

void changedir(char* dir)
{
   // TODO change directory.
   printf("Would have changed to %s\n", dir);
}
