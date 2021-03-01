#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
extern char* tzname[];

int main(void)
{
    time_t now;
    struct tm* sp;
    int result;
    result = putenv("TZ=PST8PDT");
    if (result != 0) {
        perror("The following error occurred");
    }
    (void)time(&now);

    printf("%s", ctime(&now));

    sp = localtime(&now);
    printf("%d/%d/%02d %d:%02d %s\n",
        sp->tm_mon + 1, sp->tm_mday, sp->tm_year,
        sp->tm_hour, sp->tm_min,
        tzname[sp->tm_isdst]);

    return EXIT_SUCCESS;
}
