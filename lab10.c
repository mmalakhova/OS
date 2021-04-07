#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

#define CHILD_ID 0
#define FORK_ERROR -1
#define WAIT_ERROR -1

int main(int argc, char* argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [executable] <args>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t process_id = fork();

    if (process_id == FORK_ERROR) {
        perror("Fork error: ");
        return EXIT_FAILURE;
    }

    if (process_id == CHILD_ID) {
        // v (vector). Аргументы командной строки передаются в форме вектора argv[]. Отдельные аргументы адресуются через argv [0], argv [1]... argv [n].
        // Последний аргумент (argv [n]) должен быть указателем NULL;
        // p (path). Обозначенный по имени файл ищется не только в текущем каталоге, но и в каталогах, определенных переменной среды PATH;
        execvp(argv[1], &argv[1]);
        perror("Exec error: ");
        return EXIT_FAILURE;
    }

    int status;
    pid_t ended_process_id = wait(&status);

    if (ended_process_id == WAIT_ERROR) {
        perror("Wait: ");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status)) {
        // WIFEXITED(wstatus) returns true if the child terminated normally, that is, by calling exit(3) or _exit(2), or by returning from main().
        printf("\nChild process %d ended by exit, return's code: %d\n\n", ended_process_id, WEXITSTATUS(status));
        // WEXITSTATUS(wstatus) returns the exit status of the child.
        return EXIT_SUCCESS;
    }

    if (WIFSIGNALED(status)) {
        printf("\nChild process %d ended by signal, signal: %d\n\n", ended_process_id, WTERMSIG(status));
        // WTERMSIG(wstatus) returns the number of the signal that caused the child process to terminate.
        return EXIT_SUCCESS;
    }

    return EXIT_SUCCESS;
}
