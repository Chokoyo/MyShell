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

#include "parser.h"
#include "execute.h"
#include "sighandler.h"


#define MAX_CHAR 1024 
#define MAX_STRING 30
#define CONTINUE 0


int main(int argc, char *argv[]) {
    int i = 0;
    while(1) {
        // print num of iteration
        // printf("this is the %d\n> ", i);
        // i++;

        sigint_main_init(SIGINT);
        // print prompt
        printf("$$ 3230shell ## ");

        // read input if not receive sigint
        char* input = read_input();

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
