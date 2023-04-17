#include <stdio.h>

int main(int argc, char *argv[])
{
	printf("There are %d arguemnts\n", argc);
	printf("They are: \n");
	for(int i=0; i<argc; i++){
		printf("argv %d is: %s \n", i, argv[i]);
	}
	return 0;
}
