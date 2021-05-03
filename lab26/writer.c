#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define ERROR -1
#define HAVE_INPUT_DATA 1

int initialize_writing_proc(FILE* reader)
{
    int bytes_read;
    char symbol;
    int fputc_status;

    while (HAVE_INPUT_DATA) {
        bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
        if (bytes_read == 0 || symbol == '\n') {
            break;
        }

        if (bytes_read == ERROR) {
            perror("Reading from stdin");
            exit(EXIT_FAILURE);
        }

        fputc_status = fputc(symbol, reader);
        if (fputc_status == EOF) {
            perror("Writing to pipe");
            exit(EXIT_FAILURE);
        }
    }
    pclose(reader);

    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    FILE* reader = popen("./r", "w");
    if (reader == NULL) {
        perror("Making pipe");
        return EXIT_FAILURE;
    }

    initialize_writing_proc(reader);

    pclose(reader);

    return EXIT_SUCCESS;
}
