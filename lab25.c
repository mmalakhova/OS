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
#define HAVE_WORKING_SUBPROCESS 1
#define ERROR -1

int initialize_writing_proc(int fildes[])
{
    pid_t pid = fork();
    if (pid == ERROR) {
        perror("Making subprocess for writing");
        exit(EXIT_FAILURE);
    }

    if (pid != CHILD_ID) {
        return EXIT_SUCCESS;
    }

    close(fildes[READ]);

    while (HAVE_INPUT_DATA) {
        char symbol;
        int bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
        if (bytes_read == NO_INPUT_DATA || symbol == '\n') {
            break;
        }

        if (bytes_read == ERROR) {
            perror("Reading from stdin");
            exit(EXIT_FAILURE);
        }

        int bytes_wrote = write(fildes[WRITE], &symbol, sizeof(char));
        if (bytes_wrote == ERROR) {
            perror("Writing to pipe");
            exit(EXIT_FAILURE);
        }
    }

    close(fildes[WRITE]);
    exit(EXIT_SUCCESS);
}

int initialize_reading_proc(int fildes[])
{
    int bytes_wrote = 0;

    pid_t pid = fork();
    if (pid == ERROR) {
        perror("Making subprocess for reading");
        exit(EXIT_FAILURE);
    }

    if (pid != CHILD_ID) {
        return EXIT_SUCCESS;
    }

    close(fildes[WRITE]);

    while (HAVE_DATA_FROM_PIPE) {
        char symbol;
        int bytes_read = read(fildes[READ], &symbol, sizeof(char));
        if (bytes_read == NO_INPUT_DATA) {
            break;
        }

        if (bytes_read == ERROR) {
            perror("Reading from pipe");
            exit(EXIT_FAILURE);
        }

        symbol = (char)toupper(symbol);

        bytes_wrote = write(STDOUT_FILENO, &symbol, sizeof(char));
        if (bytes_wrote == ERROR) {
            perror("Writing to stdout");
            exit(EXIT_FAILURE);
        }
    }

    write(STDOUT_FILENO, "\n", sizeof(char));
    close(fildes[READ]);
    exit(EXIT_SUCCESS);
}

void wait_subprocesses()
{
    while (HAVE_WORKING_SUBPROCESS) {
        int status;
        int ended_process_id = wait(&status);

        if (ended_process_id == ERROR && errno == ECHILD) {
            break;
        }

        if (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS) {
            fprintf(stderr, "Subprocess %d corrupt an error\n", ended_process_id);
            exit(EXIT_FAILURE);
        }

        if (WIFSIGNALED(status)) {
            fprintf(stderr, "Subprocess %d corrupt an error\n", ended_process_id);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char** argv)
{
    int fildes[2];

    int pipe_status = pipe(fildes);
    if (pipe_status == ERROR) {
        perror("Opening pipe");
        return EXIT_FAILURE;
    }

    initialize_writing_proc(fildes);
    initialize_reading_proc(fildes);

    close(fildes[READ]);
    close(fildes[WRITE]);

    wait_subprocesses();

    return EXIT_SUCCESS;
}
