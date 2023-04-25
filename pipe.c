#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

//route standard errors into file
//add error checkers like WSTATUS
int main(int argc, char *argv[])
{
	int fd[2];
	//fd[0] is the read end
	//fd[1] is the write end
	if(pipe(fd)==-1){
		return 1;
	}
	int return_code1 = fork();
	if(return_code1<0) printf("Child process 1 creation error\n");
	if(return_code1 == 0){
		dup2(fd[1], STDOUT_FILENO);
		close(fd[1]);
		close(fd[0]);
		execlp(argv[1],argv[1],NULL);
	}
	close(fd[1]);

	int fd1[2];
	if(pipe(fd1)==-1){
		return 1;
	}
	int return_code2 = fork();
	if(return_code2<0) printf("Child process 2 creation error\n");
	if(return_code2==0){
		dup2(fd[0], STDIN_FILENO);
		close(fd[0]);
		dup2(fd1[1], STDOUT_FILENO);
		close(fd1[1]);
		close(fd1[0]);
		execlp(argv[2],argv[2],NULL);
	}
	close(fd1[1]);

	int return_code3 = fork();
	if(return_code3<0) printf("Child process 3 creation error\n");
	if(return_code3==0){
		dup2(fd1[0], STDIN_FILENO);
		close(fd1[0]);
		execlp(argv[3],argv[3],NULL);
	}
	close(fd1[0]);
	int status1, status2, status3 = 0;
	waitpid(return_code1, &status1, 0);
	waitpid(return_code2, &status2, 0);
	waitpid(return_code3, &status3, 0);
	return 0;
}
