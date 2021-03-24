#include <stdio.h>
#include <stdlib.h>
#include <errno.h> // error codes
#include <sys/types.h> // lseek, open
#include <sys/stat.h> // open
#include <fcntl.h> // open
#include <unistd.h> // lseek

#define LINES_COUNT_MAX 100
#define END_LINE_NUMBER 0
#define ERROR -1
#define LSEEK_ERROR (off_t) - 1
#define SUCCESS 0
#define NOT_EOF 1
#define NOT_END_LINE 1
#define CONSOLE_INPUT 100
#define DEFAULT_FILE_NAME "text"

//return: lines count

int BuildLinesOffsetsTable(int fileDescriptor, long* linesOffsets, int* linesLengths)
{
    int currentLine = 1;
    int currentPosInLine = 0;

    linesOffsets[1] = 0L;

    while (NOT_EOF) {
        char currentChar;
        int readReturnCode = read(fileDescriptor, &currentChar, 1);

        // On error, -1 is returned, and errno is set appropriately.  In this case, it is left unspecified whether the file position (if any) changes.
        if (readReturnCode == ERROR) {
            perror("Reading Error: ");
            exit(EXIT_FAILURE);
        }

        // On  success, the number of bytes read is returned (zero indicates end of file), and the file position is advanced by this number.
        if (readReturnCode == SUCCESS) { // EOF
            break;
        }

        if (currentLine > LINES_COUNT_MAX + 1) {
            fprintf(stderr, "Error: file have more than %d lines\n", LINES_COUNT_MAX);
            exit(EXIT_FAILURE);
        }

        currentPosInLine++;

        if (currentChar != '\n') {
            continue;
        }

        linesLengths[currentLine] = currentPosInLine;
        ++currentLine;

        // Upon successful completion, lseek() returns the resulting offset
        // location as measured in bytes from the beginning of the file.
        int currentLineOffset = lseek(fileDescriptor, 0L, SEEK_CUR);

        if (currentLineOffset == LSEEK_ERROR) { // the value (off_t) -1 is returned and errno is set to indicate the error.
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
    lseek(fileDescriptor, lineOffset, SEEK_SET); // перемещаем текущую позицию чтения на lineOffset

    for (int i = 0; i < linesLength; ++i) {
        char currentChar;

        if (read(fileDescriptor, &currentChar, 1) == ERROR) {
            perror("Reading Error: ");
            exit(EXIT_FAILURE);
        }

        if (write(STDOUT_FILENO, &currentChar, 1) == ERROR) {
            perror("Writing Error: ");
            exit(EXIT_FAILURE);
        }
    }
}

//return: EXIT_SUCCESS, EXIT_FAILURE if errors occured

int RequestingUserInputLoop(int fileDescriptor, int linesCount, long* linesOffsets, int* linesLengths)
{
    printf("Lines range: [%d, %d]\n", linesCount > 0, linesCount);

    while (NOT_END_LINE) {
        int lineNumber;
        char console_input[CONSOLE_INPUT];
        char* end_ptr = NULL;
        int check_read = read(STDIN_FILENO, console_input, CONSOLE_INPUT);
        if (check_read == ERROR) {
            perror("Reading error");
            return EXIT_FAILURE;
        }
        lineNumber = strtoll(console_input, &end_ptr, 0);

        if (lineNumber == ERROR) {
            perror("Getting number error");
            return EXIT_FAILURE;
        }

        if (lineNumber == END_LINE_NUMBER) {
            return EXIT_SUCCESS;
        }

        if (lineNumber < 0 || lineNumber > linesCount) {
            perror("Out of range");
            continue;
        }

        PrintLine(fileDescriptor, linesOffsets[lineNumber], linesLengths[lineNumber]);
    }
}

int main(int argc, char* argv[])
{
    const char* file_name = (argc == 2) ? argv[1] : DEFAULT_FILE_NAME;

    //таблица отступов в файле
    long linesOffsets[LINES_COUNT_MAX + 1];
    //длины строк для каждой строки в файле
    int linesLengths[LINES_COUNT_MAX + 1];

    //файл, на имя которого указывает file_name, открывается для чтения
    //возвращаемое значение - файловый дескриптор
    int fileDescriptor = open(file_name, O_RDONLY);

    if (fileDescriptor == ERROR) {
        perror("Can't open the file");
        return EIO;
    }

    int linesCount = BuildLinesOffsetsTable(fileDescriptor, linesOffsets, linesLengths);

    RequestingUserInputLoop(fileDescriptor, linesCount, linesOffsets, linesLengths);

    close(fileDescriptor);

    return 0;
}
