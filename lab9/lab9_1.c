#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h> // execl
#include <errno.h>

#define CHILD_ID 0
#define ERROR -1

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s file_name\n", argv[0]);
		return EINVAL;
	}

	pid_t process_id = fork();

	// При неудаче родительскому процессу возвращается -1, дочерний процесс не создается
	// возможные значения errno:
	// EAGAIN - не может скопировать таблицы страниц родителя и выдерить структуру описания потомка
	// ENOMEM - ...
	if (process_id == ERROR)
	{
		perror("Fork error");
		return EXIT_FAILURE;
	}

	// При успешном завершении родителю возвращается PID процесса-потомка, а процессу-потомку возвращается 0
	if (process_id == CHILD_ID)
	{
		execl("/bin/cat", "cat", argv[1], NULL);
		perror("Execl error");
		return EXIT_FAILURE;
	}

	printf("\nPARENT MSG\n\n");

	return EXIT_SUCCESS;
}
