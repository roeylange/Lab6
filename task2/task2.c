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
void execOnce(cmdLine* p);
void execKid(cmdLine *cmd);
int checkP(cmdLine* p);

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

void execute (cmdLine* cmd){
    if (checkP(cmd)) return;
    if (cmd->next == NULL) {
        execOnce(cmd);
        return;
    }
    int c1_pid, c2_pid, p[2];
    pipe(p);
    cmdLine* cmd_next = cmd->next;

    if(flag) printf("parent_process>forking...\n");
    c1_pid = fork();
    if (c1_pid > 0) { //parent
        if(flag) printf("parent_process>created process with id: %i\n", c1_pid);
        if(flag) printf("parent_process>closing the write end of the pipe...\n");
        close(p[1]);
        if(flag) printf("parent_process>forking...\n");
        c2_pid = fork();
        if (c2_pid > 0) { //parent
            if(flag) printf("parent_process>created process with id: %i\n", c2_pid);
            if(flag) printf("parent_process>closing the read end of the pipe...\n");
            close(p[0]);
            if(flag) printf("parent_process>waiting for child processes to terminate...\n");
            if(cmd->blocking) waitpid(c1_pid, NULL, 0);
            if(cmd_next->blocking) waitpid(c2_pid, NULL, 0);
            if(flag) printf("parent_process>exiting...\n");

        } else { //child2
            if(flag) printf("child2>redirecting stdin to the read end of the pipe...\n");
            close(0);
            dup(p[0]);
            close(p[0]);
            if(flag) printf("child2>going to execute cmd: tail -n 2\n");
            redirCheck(cmd_next);
            execKid(cmd_next);
        }

    } else { //child1
        if(flag) printf("child1>redirecting stdout to the write end of the pipe...\n");
        close(1);
        dup(p[1]);
        close(p[1]);
        if(flag) printf("child1>going to execute cmd: ls -l\n");
        redirCheck(cmd);
        execKid(cmd);
    }
}

void execKid(cmdLine *cmd) {
    int check = 0;
    check = execvp(cmd->arguments[0], cmd->arguments);
    if(check<0) {
		perror("error");
		_exit(EXIT_FAILURE);
	}
    _exit(EXIT_SUCCESS);
}

void execOnce(cmdLine* cmd) {
    int check = 0;
	int pid = fork();
	if(!pid){
        redirCheck(cmd);
		check = execvp(cmd->arguments[0], cmd->arguments);
		if(check<0) {
			perror("error");
			_exit(EXIT_FAILURE);
		}
        _exit(EXIT_SUCCESS);
	}
	if (flag){
			fprintf(stderr, "PID: %d\nExecuting command: %s\n",pid,cmd->arguments[0]);
		}
	if (cmd->blocking) {
		waitpid(pid,NULL,0);
	} 
}

int checkP(cmdLine* p){
	int check = 0;
	int isHere = 0;
	if(strcmp(p->arguments[0],"quit")==0){
		isHere = 1;
		_exit(EXIT_SUCCESS);
	} else if(strcmp(p->arguments[0],"cd")==0){
		isHere = 1;
		check = chdir(p->arguments[1]);
    }
	if(check<0) {
			perror("error");
			exit(EXIT_FAILURE);
	}
	return isHere;
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


