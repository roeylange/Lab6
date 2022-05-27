#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void printPath();
void execute (cmdLine* p);
int flag = 0;

void redirCheck(cmdLine* p);

int main (int argc , char* argv[], char* envp[])
{
	
	char input[2048];
	for (int i=1; i<argc; i++){
		if(strcmp(argv[i], "-d")==0) flag=1;
	}
	while(1){
		printPath();
		fgets(input, 2048, stdin);
		cmdLine* commands = parseCmdLines(input);
		execute(commands);
		freeCmdLines(commands);
	}
  	return 0;
}

void printPath(){
	char path[PATH_MAX]; 
	char* dir = getcwd(path, 2048);
	printf("current directory: %s\n", dir);
}

void execute (cmdLine* p){
	int check = 0;
	if(strcmp(p->arguments[0],"quit")==0){
		_exit(EXIT_SUCCESS);
	}  else if(strcmp(p->arguments[0],"cd")==0){
		check = chdir(p->arguments[1]);
		if(check<0) {
			perror("error");
			exit(EXIT_FAILURE);
		}
		return;
	} 
	int pid = fork();
	if(!pid){
        redirCheck(p);
		check = execvp(p->arguments[0], p->arguments);
		if(check<0) {
			perror("error");
			_exit(EXIT_FAILURE);
		}
	}
	if (flag){
			fprintf(stderr, "PID: %d\nExecuting command: %s\n",pid,p->arguments[0]);
		}
	if (p->blocking) {
		waitpid(pid,NULL,0);
	} 
}

void redirCheck(cmdLine* p) {
    if(p->inputRedirect) {
        close(0);
        dup(open(p->inputRedirect, O_RDONLY, 0644));
    }
    if(p->outputRedirect) {
        close(1);
        dup(open(p->outputRedirect, O_WRONLY | O_CREAT, 0644));
    }
}


