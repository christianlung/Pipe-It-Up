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
	int pipes[ncommands-1][2];
	
	if(pipe(pipes[0])==-1) exit(errno);
	int first_status=0;
	int first_return = fork();
	if(first_return<0) exit(errno);
	if(first_return==0){
		if(dup2(pipes[0][1], STDOUT_FILENO)<0) exit(errno);
		close(pipes[0][1]);
		close(pipes[0][0]);
		if(execlp(argv[1],argv[1],NULL)==-1) exit(errno);
	}
	waitpid(first_return, &first_status,0);
	close(pipes[0][1]);
	if(WEXITSTATUS(first_status)) return WEXITSTATUS(first_status);

	int status=0;
	int return_code=0;
	for(int process=1; process<ncommands-1; process++){
		if(pipe(pipes[process])==-1) exit(errno);
		return_code = fork();
		if(return_code<0) exit(errno);
		if(return_code==0){
			if(dup2(pipes[process-1][0], STDIN_FILENO)<0) exit(errno);
			close(pipes[process-1][0]);
			if(dup2(pipes[process][1], STDOUT_FILENO)<0) exit(errno);
			close(pipes[process][1]);
			close(pipes[process][0]);
			if(execlp(argv[process+1],argv[process+1],NULL)==-1) exit(errno);
		}
		waitpid(return_code, &status,0);
		close(pipes[process-1][0]);
		close(pipes[process][1]);
		if(WEXITSTATUS(status)) return WEXITSTATUS(status);
	}

	int last_status=0;
	int last_return = fork();
	if(last_return<0) exit(errno);
	if(last_return==0){
		if(dup2(pipes[ncommands-2][0], STDIN_FILENO)<0) exit(errno);
		close(pipes[ncommands-2][0]);
		if(execlp(argv[ncommands], argv[ncommands],NULL)==-1) exit(errno);
	}
	waitpid(last_return,&last_status,0);
	close(pipes[ncommands-2][0]);
	if(WEXITSTATUS(last_status)) return WEXITSTATUS(last_status);
	return 0;
}
