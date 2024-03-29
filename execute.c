/* execute.c - code used by small shell to execute commands */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "mysh.h"

#define EXIT "exit"
#define CD "cd"
void changedir(char*);

int execute(int background, char *argv[])
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

   if (strcmp(argv[0], CD) == 0){
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
      exitShell(1);
   }
   else {
      if (background)
         printf("Started process %d.\n", pid);
      else if ( waitpid(pid, &child_info, 0) == -1 )
         perror("wait");
   }
   return child_info;
}

void handleSigChild (int signum)
{
   int pid;
   int status;

   // Wait for all children that haven't exited.
   while((pid = waitpid(-1, &status, WNOHANG)) > 0 ){
      printf("Process %d exited with status %d.\n", pid, status);
      // TODO Buffer output to display at appropriate time.
   }
}

void changedir(char* dir)
{
   char* buf;
   int dirlen = strlen(dir);

   // Test for home
   if (dir[0] == '~'){
      char* home = getenv("HOME");
      int homelen =  strlen(home);
      int buflen = homelen+dirlen;
      buf = (char*) emalloc(buflen + 1);
      memcpy(buf, home, homelen);
      memcpy(&buf[homelen], &dir[1], dirlen);
   }
   else if (dir[0] != '/'){
      char* pwd = getcwd(0,0);
      int pwdlen =  strlen(pwd);
      buf = (char*) emalloc(pwdlen + dirlen + 2);
      memcpy(buf, pwd, pwdlen);
      buf[pwdlen] = '/';
      memcpy(&buf[pwdlen + 1], dir, dirlen + 1);
   } else {
      buf = (char*) emalloc(dirlen + 1);
      memcpy(buf, dir, dirlen + 1);
   }

   // Get real path.
   char* path = realpath(buf, NULL);
   free(buf);

   // Change directory.
   if (chdir(path) != 0){
      printf("Could not change directory.\n");
   }

   free(path);
}
