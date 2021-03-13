#include <stdio.h>
#include <stdlib.h>
#include <errno.h>     // error codes
#include <sys/types.h> // lseek, open
#include <sys/stat.h>  // open
#include <fcntl.h>     // open
#include <unistd.h>    // lseek

#define LINES_COUNT_MAX 100
#define END_LINE_NUMBER 0
#define ERROR -1
#define SUCCESS 0
#define DEFAULT_FILE_NAME "text"


 //return: lines count

int BuildLinesOffsetsTable(int fileDescriptor, long *linesOffsets, int *linesLengths)
{
    int currentLine = 1;
    int currentPosInLine = 0;

    linesOffsets[1] = 0L;

    while (1)
    {
        char currentChar;
        int readReturnCode = read(fileDescriptor, &currentChar, 1);

        // On error, -1 is returned, and errno is set appropriately.  In this case, it is left unspecified whether the file position (if any) changes.
        if (readReturnCode == ERROR)
        {
            perror("Reading Error: ");
            exit(EXIT_FAILURE);
        }

        // On  success, the number of bytes read is returned (zero indicates end of file), and the file position is advanced by this number.
        if (readReturnCode == SUCCESS)
        { // EOF
            break;
        }

        if (currentLine > LINES_COUNT_MAX + 1)
        {
            fprintf(stderr, "Error: file have more than %d lines\n", LINES_COUNT_MAX);
            exit(EXIT_FAILURE);
        }

        currentPosInLine++;

        if (currentChar != '\n')
        {
            continue;
        }

        linesLengths[currentLine] = currentPosInLine;
        ++currentLine;

        // Upon successful completion, lseek() returns the resulting offset
        // location as measured in bytes from the beginning of the file.
        int currentLineOffset = lseek(fileDescriptor, 0L, SEEK_CUR);

        if (currentLineOffset == ERROR)
        { // the value (off_t) -1 is returned and errno is set to indicate the error.
            perror("Lseek Error: ");
            exit(EXIT_FAILURE);
        }

        linesOffsets[currentLine] = currentLineOffset;
        currentPosInLine = 0;
    }

    return currentLine - 1;
}

void PrintLine(int fileDescriptor, int lineOffset, int linesLength)
{
    lseek(fileDescriptor, lineOffset, SEEK_SET); // move to requested line

    for (int i = 0; i < linesLength; ++i)
    {
        char currentChar;
        if (read(fileDescriptor, &currentChar, 1) == ERROR)
        {
            perror("Reading Error: ");
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, &currentChar, 1) == ERROR)
        {
            perror("Writing Error: ");
            exit(EXIT_FAILURE);
        }
    }
}



 //return: EXIT_SUCCESS, EXIT_FAILURE if errors occured

int RequestingUserInputLoop(int fileDescriptor, int linesCount, long *linesOffsets, int *linesLengths)
{
    printf("Lines range: [%d, %d]\n", linesCount > 0, linesCount);
    do
    {
        printf("$ ");

        int lineNumber;
        char ending;

        if (scanf("%d%c", &lineNumber, &ending) != 2 || ending != '\n')
        {
            fprintf(stderr, "Format Error\n");
            return EXIT_FAILURE;
        }

        if (lineNumber == END_LINE_NUMBER)
        {
            return EXIT_SUCCESS;
        }

        if (lineNumber < 0 || lineNumber > linesCount)
        {
            fprintf(stderr, "Out Of Range Error: lineNumber %d out of range [%d, %d]\n", lineNumber, linesCount > 0, linesCount);
            continue;
        }

        PrintLine(fileDescriptor, linesOffsets[lineNumber], linesLengths[lineNumber]);
    } while (1);
}


int main(int argc, char *argv[])
{
    const char* file_name = (argc == 2) ? argv[1] : DEFAULT_FILE_NAME;

    //таблица отступов в файле
    long linesOffsets[LINES_COUNT_MAX + 1];
    //длины строк для каждой строки в файле
    int linesLengths[LINES_COUNT_MAX + 1];

   //файл, на имя которого указывает file_name, открывается для чтения
   //возвращаемое значение - файловый дескриптор
    int fileDescriptor = open(file_name, O_RDONLY);

    if (fileDescriptor == ERROR)
    {
        perror(argv[0]);
        return EIO;
    }

    int linesCount = BuildLinesOffsetsTable(fileDescriptor, linesOffsets, linesLengths);

    return RequestingUserInputLoop(fileDescriptor, linesCount, linesOffsets, linesLengths);
}
