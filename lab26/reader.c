#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>

#define HAVE_INPUT_DATA 1
#define ERROR -1

int main(int argc, char** argv)
{
    while (HAVE_INPUT_DATA) {
        char symbol;
        int bytes_read = read(STDIN_FILENO, &symbol, sizeof(char));
        if (bytes_read == ERROR) {
            perror("Reading from stdin");
            return EXIT_FAILURE;
        }

        if (bytes_read == 0) {
            break;
        }

        symbol = (char)toupper(symbol);

        int bytes_wrote = write(STDOUT_FILENO, &symbol, sizeof(char));
        if (bytes_wrote == ERROR) {
            perror("Writing to stdout");
            return EXIT_FAILURE;
        }
    }

    write(STDOUT_FILENO, "\n", sizeof(char));

    return EXIT_SUCCESS;
}
