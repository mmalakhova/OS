#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define READ 0
#define WRITE 1
#define CHILD_ID 0
#define NO_INPUT_DATA 0
#define HAVE_INPUT_DATA 1
#define HAVE_DATA_FROM_PIPE 1
#define ERROR -1

int initialize_writing_proc(int fildes[])
{
    while (HAVE_INPUT_DATA) {
        char symbol;
        int bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
        if (bytes_read == NO_INPUT_DATA || symbol == '\n') {
            break;
        }

        if (bytes_read == ERROR) {
            perror("Reading from stdin");
            return EXIT_FAILURE;
        }

        int bytes_wrote = write(fildes[WRITE], &symbol, sizeof(char));
        if (bytes_wrote == ERROR) {
            perror("Writing to pipe");
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

int initialize_reading_proc(int fildes[])
{
    int bytes_wrote = 0;
    while (HAVE_DATA_FROM_PIPE) {
        char symbol;
        int bytes_read = read(fildes[READ], &symbol, sizeof(char));
        if (bytes_read == NO_INPUT_DATA) {
            break;
        }

        if (bytes_read == ERROR) {
            perror("Reading from pipe");
            return EXIT_FAILURE;
        }

        symbol = (char)toupper(symbol);

        bytes_wrote = write(STDOUT_FILENO, &symbol, sizeof(char));
        if (bytes_wrote == ERROR) {
            perror("Writing to stdout");
            return EXIT_FAILURE;
        }
    }
    write(STDOUT_FILENO, "\n", sizeof(char));
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    int fildes[2];

    int pipe_status = pipe(fildes);
    if (pipe_status == ERROR) {
        perror("Opening pipe");
        return EXIT_FAILURE;
    }

    pid_t writing_subprocess = fork();
    if (writing_subprocess == ERROR) {
        close(fildes[READ]);
        close(fildes[WRITE]);
        return EXIT_FAILURE;
    }
    if (writing_subprocess == CHILD_ID) {
        close(fildes[READ]);
        int status = initialize_writing_proc(fildes);
        close(fildes[WRITE]);
        return status;
    }

    pid_t reading_subprocess = fork();
    if (reading_subprocess == ERROR) {
        close(fildes[READ]);
        close(fildes[WRITE]);
        return EXIT_FAILURE;
    }
    if (reading_subprocess == CHILD_ID) {
        close(fildes[WRITE]);
        int status = initialize_reading_proc(fildes);
        close(fildes[READ]);
        return status;
    }

    close(fildes[READ]);
    close(fildes[WRITE]);

    if (waitpid(writing_subprocess, NULL, 0) == (pid_t)ERROR) {
        perror("WAITPID ERROR");
        return EXIT_FAILURE;
    }
    if (waitpid(reading_subprocess, NULL, 0) == (pid_t)ERROR) {
        perror("WAITPID ERROR");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
