#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#define PUTENV_SUCCESS 0
#define EXEC_ERROR -1

extern char** environ;

int execvpe(char* file, char* argv[], char* envp[]) {
    if(file == NULL) {
        perror("NULL pointer to file");
        return EXIT_FAILURE;
    }
    char** savedEnviron = environ;

    if (execvp(file, argv) == EXEC_ERROR){
	environ = savedEnviron;
	perror("exec error");
    	return EXIT_FAILURE;
	}
}

int main(int argc, char* argv[], char* envp[]) {
    if(argc < 2) {
        fprintf(stderr, "Usage: %s [executable] <args>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if(envp == NULL) {
        perror("Environment is not set");
    }

    if(putenv("TEST=os_lab11") != PUTENV_SUCCESS){
        perror("putenv error");
	return EXIT_FAILURE;
    }

    execvpe(argv[1], &argv[1], envp);
    perror("exec error");

    return EXIT_FAILURE;
}
