#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <limits.h>
#include <glob.h>

#define GLOB_SUCCESS 0

int errFunc(const char* epath, int errno)
{
    fprintf(stderr, "Glob error. Couldn't check for matching: %s\n", epath);
    return EXIT_FAILURE;
}

int print_matched_files(char* mask)
{
    glob_t glob_results;

    // Параметр flags является нулем или поразрядным ИЛИ нескольких следующих символьных констант, которые меняют ход работы glob():
    // GLOB_NOCHECK - если не найдено совпадений по шаблону, то возвращать в качестве результата заданный шаблон;
    int glob_status = glob(mask, GLOB_NOCHECK, errFunc, &glob_results);

    // При нормальном завершении glob() возврашает ноль.
    // Другие возможные возвращаемые значения: GLOB_NOSPACE, GLOB_ABORTED, GLOB_NOMATCH
    if (glob_status != GLOB_SUCCESS) {
        perror("GLOB");
        globfree(&glob_results);
        return EXIT_FAILURE;
    }

    // При успешном завершении работы pglob->gl_pathc содержит количество найденных совпадений имен, а pglob->gl_pathv указывает на список найденных имен.
    // Указатель, который находится непосредственно за последним в списке имен, равен NULL.
    int matching_index;
    for (matching_index = 0; matching_index < glob_results.gl_pathc; ++matching_index) {
        printf("%s\n", glob_results.gl_pathv[matching_index]);
    }

    globfree(&glob_results);
    return EXIT_SUCCESS;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s [executable] <args>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int i, print_info;
    for (i = 1; i < argc; ++i)
    {
       print_info = print_matched_files(argv[i]);
       if (print_info == EXIT_FAILURE) return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
