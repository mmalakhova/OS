#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define ERROR -1
#define SUCCESS 0

typedef struct LinkedList
{
    char *value;
    struct LinkedList *next;
} LinkedList;

LinkedList *MakeNode(char *value)
{
    LinkedList *newNode;
    if ((newNode = (LinkedList *)malloc(sizeof(LinkedList))) == NULL)
    {
        return NULL;
    }

    if ((newNode->value = (char *)malloc(strlen(value) + 1)) == NULL)
    {
        return NULL;
    }

    strcpy(newNode->value, value);
    newNode->next = NULL;

    return newNode;
}


int ReadLinesToLinkedList(LinkedList *head)
{
    char buffer[BUFSIZ] = ""; 
    LinkedList *move = head;

    do
    {
        printf("> ");
        scanf("%s", buffer);

        if ((move->next = MakeNode(buffer)) == NULL)
        {
            return ERROR;
        }

        move = move->next;
    } while (buffer[0] != '.');

    return SUCCESS;
}

void PrintLines(LinkedList *head)
{
    LinkedList *move = NULL;

    for (LinkedList *move = head->next; move != NULL; move = move->next)
    {
        puts(move->value);
    }
}

int main(int argc, char **argv)
{
    LinkedList *head = NULL;

    if ((head = (LinkedList *)malloc(sizeof(LinkedList))) == NULL)
    {
        perror(argv[0]);
        return ENOMEM; // ENOMEM stands for Error NO MEMory
    }

    head->next = NULL;

    if (ReadLinesToLinkedList(head) == ERROR)
    {
        perror(argv[0]);
        return ENOMEM;
    }

    PrintLines(head);

    return EXIT_SUCCESS;
}
