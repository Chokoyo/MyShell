#define MAX_CHAR 1024
#define MAX_STRING 30

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

char **parse(char *input)
{
    // char** all_tokens = malloc(all_tokens_buffer * sizeof(char*));
    // remove new line character
    input[strlen(input) - 1] = '\0';
    char **instr = malloc(MAX_STRING * sizeof(char*));
    char *ptr = strtok(input, " ");
    int i = 0;
    while (ptr != NULL)
    {
        instr[i++] = ptr;
        ptr = strtok(NULL, " ");
    }
    instr[i] = NULL;

    // for (int j = 0; j < i; j++)
    // {
    //     printf("%s\n", instr[j]);
    // }
    return instr;
}

int get_args_length(char** args) {
    int i = 0;
    while (args[i] != NULL) {
        i++;
    }
    return i;
}