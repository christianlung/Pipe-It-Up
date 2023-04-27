#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if(argc==1) exit(EINVAL);
	if(argc==2){
		execlp(argv[1],argv[1],NULL);
		exit(errno);
	}

	int ncommands = argc-1;
	int status[ncommands];
	int return_codes[ncommands];
	int pipes[ncommands-1][2];

	if(pipe(pipes[0])==-1) exit(errno);
	return_codes[0] = fork();
	if(return_codes[0]<0) exit(errno);
	if(return_codes[0]==0){
		if(dup2(pipes[0][1], STDOUT_FILENO)<0) exit(errno);
		close(pipes[0][1]);
		close(pipes[0][0]);
		if(execlp(argv[1],argv[1],NULL)==-1) exit(errno);
	}
	waitpid(return_codes[0], &status[0],0);
	close(pipes[0][1]);
	if(WEXITSTATUS(status[0])) return WEXITSTATUS(status[0]);

	for(int process=1; process<ncommands-1; process++){
		if(pipe(pipes[process])==-1) exit(errno);
		return_codes[process] = fork();
		if(return_codes[process]<0) exit(errno);
		if(return_codes[process]==0){
			if(dup2(pipes[process-1][0], STDIN_FILENO)<0) exit(errno);
			close(pipes[process-1][0]);
			if(dup2(pipes[process][1], STDOUT_FILENO)<0) exit(errno);
			close(pipes[process][1]);
			close(pipes[process][0]);
			if(execlp(argv[process+1],argv[process+1],NULL)==-1) exit(errno);
		}
		waitpid(return_codes[process], &status[process],0);
		close(pipes[process-1][0]);
		close(pipes[process][1]);
		if(WEXITSTATUS(status[process])) return WEXITSTATUS(status[process]);
	}

	return_codes[ncommands-1] = fork();
	if(return_codes[ncommands-1]<0) exit(errno);
	if(return_codes[ncommands-1]==0){
		if(dup2(pipes[ncommands-2][0], STDIN_FILENO)<0) exit(errno);
		close(pipes[ncommands-2][0]);
		if(execlp(argv[ncommands], argv[ncommands],NULL)==-1) exit(errno);
	}
	waitpid(return_codes[ncommands-1],&status[ncommands-1],0);
	close(pipes[ncommands-2][0]);
	if(WEXITSTATUS(status[ncommands-1])) return WEXITSTATUS(status[ncommands-1]);
	return 0;
}
