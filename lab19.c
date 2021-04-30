#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <glob.h>

#define GLOB_SUCCESS 0
#define NOT_END_OF_INPUT 1

void read_template(char* mask)
{
    int length = 0;

    while (NOT_END_OF_INPUT) {
        if (length > PATH_MAX) {
            fprintf(stderr, "Too long template. Max file name length is %d\n", PATH_MAX);
            exit(EXIT_FAILURE);
        }

        char read_symb = (char)fgetc(stdin);

        if (read_symb == EOF || read_symb == '\n') {
            break;
        }

        if (read_symb == '/') {
            fprintf(stderr, "'/' must not occur in the template.\n");
            exit(EXIT_FAILURE);
        }

        mask[length++] = read_symb;
    }

    if (length == 0) {
        mask[length++] = '*';
    }
    mask[length] = '\0';
}

void print_matched_files(char* mask)
{
    glob_t glob_results;

    // Параметр flags является нулем или поразрядным ИЛИ нескольких следующих символьных констант, которые меняют ход работы glob():
    // GLOB_NOCHECK - если не найдено совпадений по шаблону, то возвращать в качестве результата заданный шаблон;
    int glob_status = glob(mask, GLOB_NOCHECK, NULL, &glob_results);

    // При нормальном завершении glob() возврашает ноль.
    // Другие возможные возвращаемые значения: GLOB_NOSPACE, GLOB_ABORTED, GLOB_NOMATCH
    if (glob_status != GLOB_SUCCESS) {
        perror("GLOB");
        globfree(&glob_results);
        exit(EXIT_FAILURE);
    }

    // При успешном завершении работы pglob->gl_pathc содержит количество найденных совпадений имен, а pglob->gl_pathv указывает на список найденных имен.
    // Указатель, который находится непосредственно за последним в списке имен, равен NULL.
    int matching_index;
    for (matching_index = 0; matching_index < glob_results.gl_pathc; ++matching_index) {
        printf("%s\n", glob_results.gl_pathv[matching_index]);
    }

    globfree(&glob_results);
}

int main(int argc, char* argv[])
{
    if (argc > 1) {
        fprintf(stderr, "Usage: %s \n", argv[0]);
        return EXIT_FAILURE;
    }

    char mask[PATH_MAX + 1];
    read_template(mask);
    print_matched_files(mask);

    return EXIT_SUCCESS;
}
