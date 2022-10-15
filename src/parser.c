#define MAX_CHAR 1024
#define MAX_STRING 30
#define MAX_CMD 5

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parser.h"

char **parse(char *input)
{
    if (input == NULL)
    {
        // return empty array
        char **empty = malloc(sizeof(char *));
        empty[0] = NULL;
        return empty;
    }
    // check recieve sigint
    // char** all_tokens = malloc(all_tokens_buffer * sizeof(char*));
    // remove new line character
    // input[strlen(input) - 1] = '\0';
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

// char ***parse_command(char *instr, char delimiter)
// {
//     char ***all_cmd = malloc(MAX_CMD * sizeof(char**));
//     int len = strlen(instr);
//     int index = 0;
    
//     char **curr_cmd = malloc(MAX_STRING * sizeof(char*));
//     for (int i = 0; i < len; i++)
//     {
//         curr_cmd[i] = malloc(MAX_CHAR * sizeof(char));

//         if (strlen(instr[i]) == 1 && instr[0] == delimiter)
//         {
            
//         }
//     }
// }


// parse the instruction into multiple commands if there is a pipe
char ***parse_command(char **instr, char delimiter) {
    char ***all_cmd = malloc(MAX_CMD * sizeof(char**));
    int len = get_args_length(instr);
    int index = 0;
    int cmd_index = 0;
    char **curr_cmd = malloc(MAX_STRING * sizeof(char*));
    for (int i = 0; i < len; i++) {
        if (strcmp(instr[i], "|") == 0) {
            curr_cmd[cmd_index] = NULL;
            all_cmd[index++] = curr_cmd;
            curr_cmd = malloc(MAX_STRING * sizeof(char*));
            cmd_index = 0;
        } else {
            curr_cmd[cmd_index++] = instr[i];
        }
    }
    curr_cmd[cmd_index] = NULL;
    all_cmd[index] = curr_cmd;
    return all_cmd;
}

int get_args_length(char** args) {
    int i = 0;
    while (args[i] != NULL) {
        i++;
    }
    return i;
}

char* get_cmd_name(char* args) {
    char* cmd_name = malloc(MAX_CHAR * sizeof(char));
    int len = 0;
    // find position of last slash
    while (args[len] != '\0') {
        len++;
    }
    int i = len - 1;
    while (args[i] != '/') {
        if (i == 0) {
            return args;
        }
        i--;
    }
    for (int j = i + 1; j < len; j++) {
        cmd_name[j - i - 1] = args[j];
    }
    return cmd_name;
}

char* read_input() {
    char *input = malloc(MAX_CHAR * sizeof(char));
    memset(input, 0, MAX_CHAR);
    char c;
    int i = 0;
    while ((c = getchar()) != EOF && c != '\n' && (c >= 0 && c < 128)) {
        input[i++] = c;
        // printf("%c", c);
    }
    // printf("\n");
    input[i] = '\0';
    return input;
}