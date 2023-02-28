// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – all requirements are implemented, including bonus

#define MAX_CHAR 1024 
#define MAX_STRING 30

#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#include "sllist.h" // linked list data structure to store background
#include "parser.h" // reading and parsing the input
#include "execute.h" // execute the parsed instruction
#include "sighandler.h" // signal handlers

// Global variables
bool print_background_info;
bool print_prompt;
SLList background_list;

void prompt();

// Main function
int main(int argc, char *argv[]) {

    // Initializa variable
    print_prompt = true;
    createSLList(&background_list, (int) sizeof(Job));
    
    while(1) {
        // Print prompt
        prompt();
        
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

void prompt() {
    sigint_main_init(SIGINT);
    if (print_prompt) {
            printf("$$ 3230shell ## ");
    }
    print_prompt = true;
    print_background_info = true;
}
