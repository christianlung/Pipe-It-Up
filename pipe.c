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
		exit(1);
	}

	//start general (assume 2 commands)///////////
	int ncommands = argc-1;
	int status[ncommands];
	int return_codes[ncommands];
	int pipes[ncommands-1][2];

	if(pipe(pipes[0])==-1) exit(1);
	return_codes[0] = fork();
	if(return_codes[0]<0) exit(1);
	if(return_codes[0]==0){
		if(dup2(pipes[0][1], STDOUT_FILENO)<0) exit(1);
		close(pipes[0][1]);
		close(pipes[0][0]);
		if(execlp(argv[1],argv[1],NULL)==-1) exit(1);
	}
	waitpid(return_codes[0], &status[0],0);
	close(pipes[0][1]);

	for(int process=1; process<ncommands-1; process++){
		if(pipe(pipes[process])==-1) exit(1);
		return_codes[process] = fork();
		if(return_codes[process]<0) exit(1);
		if(return_codes[process]==0){
			if(dup2(pipes[process-1][0], STDIN_FILENO)<0) exit(1);
			close(pipes[process-1][0]);
			if(dup2(pipes[process][1], STDOUT_FILENO)<0) exit(1);
			close(pipes[process][1]);
			close(pipes[process][0]);
			if(execlp(argv[process+1],argv[process+1],NULL)==-1) exit(1);
		}
		waitpid(return_codes[process], &status[process],0);
		close(pipes[process-1][0]);
		close(pipes[process][1]);
	}

	return_codes[ncommands-1] = fork();
	if(return_codes[ncommands-1]<0) exit(1);
	if(return_codes[ncommands-1]==0){
		if(dup2(pipes[ncommands-2][0], STDIN_FILENO)<0) exit(1);
		close(pipes[ncommands-2][0]);
		if(execlp(argv[ncommands], argv[ncommands],NULL)==-1) exit(1);
	}
	waitpid(return_codes[ncommands-1],&status[ncommands-1],0);
	close(pipes[ncommands-2][0]);
	//end general//////////////////////////////////


	/*
	int status1, status2, status3, status4 = 0;
	int fd[2];
	//fd[0] is the read end
	//fd[1] is the write end
	if(pipe(fd)==-1) exit(1);
	int return_code1 = fork();
	if(return_code1<0) exit(1);
	if(return_code1 == 0){
		if(dup2(fd[1], STDOUT_FILENO)<0) exit(1);
		close(fd[1]);
		close(fd[0]);
		execlp(argv[1],argv[1],NULL);
	}
	close(fd[1]);

	waitpid(return_code1, &status1, 0);
	// printf("Child process 1 exits with code: %d\n", WEXITSTATUS(status1));

	int fd1[2];
	if(pipe(fd1)==-1) exit(1);
	int return_code2 = fork();
	if(return_code2<0) exit(1);
	if(return_code2==0){
		if(dup2(fd[0], STDIN_FILENO)<0) exit(1);
		close(fd[0]);
		if(dup2(fd1[1], STDOUT_FILENO)<0) exit(1);
		close(fd1[1]);
		close(fd1[0]);
		execlp(argv[2],argv[2],NULL);
	}
	close(fd[0]);
	close(fd1[1]);

	waitpid(return_code2, &status2, 0);
	// printf("Child process 2 exits with code: %d\n", WEXITSTATUS(status2));

	int fd2[2];
	if(pipe(fd2)==-1) exit(1);
	int return_code3 = fork();
	if(return_code3<0) exit(1);
	if(return_code3==0){
		if(dup2(fd1[0], STDIN_FILENO)<0) exit(1);
		close(fd1[0]);
		if(dup2(fd2[1],STDOUT_FILENO)<0) exit(1);
		close(fd2[1]);
		close(fd2[0]);
		execlp(argv[3],argv[3],NULL);
	}
	close(fd1[0]);
	close(fd2[1]);

	waitpid(return_code3, &status3, 0);
	// printf("Child process 3 exits with code: %d\n", WEXITSTATUS(status3));

	int return_code4 = fork();
	if(return_code4<0) exit(1);
	if(return_code4==0){
		if(dup2(fd2[0], STDIN_FILENO)<0) exit(1);
		close(fd2[0]);
		execlp(argv[4],argv[4],NULL);
	}
	close(fd2[0]);

	waitpid(return_code4, &status4, 0);
	*/
	return 0;
}
