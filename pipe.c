#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int fds[2];
	//fds[0] is the read end
	//fds[1] is the write end
	if(pipe(fds)==-1){
		return 1;
	}
	int return_code1 = fork();
	if(return_code1<0) printf("Child process 1 creation error\n");
	if(return_code1 == 0){
		dup2(fds[1], STDOUT_FILENO);
		close(fds[1]);
		close(fds[0]);
		execlp(argv[1],argv[1],NULL);
	}

	int return_code2 = fork();
	if(return_code2<0) printf("Child process 2 creation error\n");
	if(return_code2==0){
		dup2(fds[0], STDIN_FILENO);
		close(fds[0]);
		close(fds[1]);
		execlp(argv[2],argv[2],NULL);
	}

	close(fds[0]);
	close(fds[1]);

	int status1, status2 = 0;
	waitpid(return_code1, &status1, 0);
	waitpid(return_code2, &status2, 0);
	// printf("Child process 1 extis with code: %d \n", WEXITSTATUS(status1));
	// printf("Child process 2 exits with code: %d \n", WEXITSTATUS(status2));
	return 0;
}
