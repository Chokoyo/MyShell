// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – describe how much you have completed; whether you have implemented the bonus part.

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <setjmp.h>

#include "parser.h"
#include "execute.h"
#include "sighandler.h"


#define MAX_CHAR 1024 
#define MAX_STRING 30
#define CONTINUE 0

jmp_buf jmpbuf;
bool print_background_info;
bool print_prompt;

int main(int argc, char *argv[]) {
    int i = 0;
    print_prompt = true;
    while(1) {
        
        // print num of iteration
        // printf("this is the %d\n> ", i);

        setjmp(jmpbuf);
        sigint_main_init(SIGINT);
        // print prompt
        if (print_prompt) {
            printf("$$ 3230shell ## ");
        }
        print_prompt = true;
    
        print_background_info = true;
        // read input if not receive sigint
        char* input = read_input();
        
        // char *input = malloc(MAX_CHAR * sizeof(char));
        // fflush(stdout);
        // fflush(stdin);
        // fgets(input, MAX_CHAR, stdin);
        // input[strlen(input) - 1] = '\0';
        // printf("%s\n", input);

        // parse the instruction
        // const char *instr[MAX_STRING];
        // char *ptr = strtok(input, " ");
        // int i = 0;
        // while(ptr != NULL) {
        //     instr[i++] = ptr;
        //     ptr = strtok(NULL, " ");
        // }
        // instr[i] = NULL;
        char** instr = parse(input);

        // execute the instruction
        execute(instr);

        // free the memory
        free(input);
        free(instr);
    }
}
