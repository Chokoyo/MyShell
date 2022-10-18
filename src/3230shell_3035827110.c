// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – all requirements are implemented, including bonus

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <setjmp.h>

#include "sllist.h"
#include "parser.h"
#include "execute.h"
#include "sighandler.h"


#define MAX_CHAR 1024 
#define MAX_STRING 30
#define CONTINUE 0

// global variables
jmp_buf jmpbuf;
bool print_background_info;
bool print_prompt;
SLList background_list;

// main function
int main(int argc, char *argv[]) {
    int i = 0;
    print_prompt = true;
    createSLList(&background_list, (int) sizeof(Job));
    
    while(1) {
        setjmp(jmpbuf);
        sigint_main_init(SIGINT);

        // Print prompt
        if (print_prompt) {
            printf("$$ 3230shell ## ");
        }
        print_prompt = true;
        print_background_info = true;
        
        // Read input
        char* input = read_input();
        
        // Parse instruction
        char** instr = parse(input);

        // Execute instruction
        execute(instr);

        // Free the memory
        free(input);
        free(instr);
    }
}
