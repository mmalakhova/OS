#include <errno.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define DEFAULT_FILE_NAME "file"


FILE* open_file(const char* file_name) {
    FILE* file_for_return = fopen(file_name, "r");
    if (file_for_return == NULL) {
        fprintf(stderr, "Can't open %s file.\n", file_name);
        perror("fopen");
    }
    return file_for_return;
}

int main(int argc, char* argv[]) {
    FILE* input_file;

   const char* file_name = (argc == 2) ? argv[1] : DEFAULT_FILE_NAME;

    for (int index = 0; index < 2; ++index) {
       
        fprintf(stdout, "Real UID:%u\nEffective UID:%u\n", getuid(), geteuid());
        input_file = open_file(file_name);
        if (input_file == NULL) {
            return (EXIT_FAILURE);//выход из фун-и main с аргументом EXIT_FAILURE(макрос, указывающий на состояние выполнения программы)
        }
      if (setuid(getuid()) < 0) {
            fputs("Can't change euid\n", stderr);
            perror(argv[0]);
            return (EXIT_FAILURE);
        }

 if (fclose(input_file) == EOF) {
            fprintf(stderr, "Can't close %s\n", file_name);
            perror(argv[0]);
            return (EXIT_FAILURE);
        }
    }

    return (EXIT_SUCCESS);
}

