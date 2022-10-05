// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – describe how much you have completed; whether you have implemented the bonus part.

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "execute.h"


#define MAX_CHAR 1024 
#define MAX_STRING 30

int main(int argc, char *argv[]) {
   
   while(1) {
        // get input
        printf("$$ 3230shell ## ");
        char *input = NULL;
        size_t len = 0;
        getline(&input, &len, stdin);
        
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
        // for (int j = 0; j < 5; j++) {
        //     printf("%s\n", instr[j]);
        // }

        // execute the instruction
        execute(instr);

        
        // free the memory
        free(input);
        free(instr);
        
    }
}

