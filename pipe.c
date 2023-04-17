#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	// printf("I am lazy, letting other program to ls the directory\n\n");
	// printf("I will just wait for their report \n\n");
	// execlp(argv[1], argv[1], NULL);
	// printf("They Finished; Done!\n");
	int return_code = fork();
	if(return_code == 0){
		printf("This is the child process \n");
		execlp(argv[1],argv[1],NULL);
		exit(0);
	}
	else if(return_code>0){
		printf("i am lazy parent, letting my child execute a command\n");
		printf("I will wait\n");
		int pid = return_code;
		int status = 0;
		waitpid(pid, &status, 0);
		printf("Child process exits with code: %d \n", WEXITSTATUS(status));
	}
	else{
		printf("child process creation error\n");
	}
	printf("They finished; Done!\n");
	return 0;
}
