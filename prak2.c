#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/stat.h>		// open()
#include <signal.h>			// signal()
#include <fcntl.h>			// open()
#include <stdio.h>			// printf(), ...
#include <time.h>				// time(), ...
#include <string.h>			// strtok()
#define MAXLINE 100
#define MOD "exit with CTR C"
#define BUFFERSIZE 30


int read_command(char *command, char *parameters[]) { // prompt for user input and read a command line 
   fprintf(stdout, "$ ");
   char input[BUFFERSIZE];
   char* token;
   int index = 0;
   

   fgets(input, BUFFERSIZE, stdin);
   token = strtok(input," ");
 
  while (token != NULL){
   parameters[index] = token;
   ++index;

token = strtok(NULL," ");
}

parameters[index] = NULL;


   return(index);
} 

int main(int argc, char *argv[]) {
    int childPid;
    int status;
    char command[MAXLINE];
    char *parameters[MAXLINE];
    int noParams = 0;

    while (1) {
        noParams = read_command(command, parameters); 
        printf("sdsd: %d\n",noParams);// read user input
      
        if (noParams == 0) {
            fprintf(stderr, "no command ?!\n");
            exit(1); 
        }
        if ((childPid = fork()) == -1) { // create process
            fprintf(stderr, "can't fork!\n");
            exit(2);
        } else if (childPid == 0) { // child process
           execvp(command, parameters); // executes command
           exit(3);
        } else { // father 
	      waitpid(childPid, &status, WUNTRACED | WCONTINUED);
        } 
    }
    
    exit(0);
}
