#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "execute.h"
#include "sighandler.h"
#include "parser.h"



void execute(char** args) {
    exit_on_command(args);
    int timex_mode = is_timex_command(args); // 0 if not timex, 1 if timex
    

    execute_single(args, timex_mode);

}

void execute_single(char** args, int timex_mode) {
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        // printf("child process\n");

        // signal handlers init
        sighandler_init();
        execvp(args[0], args);

        // error handling
        print_error_message(args);
        exit(-1);
    } else {
        // parent process
        // if in timex mode, use wait4() to get rusage
        if (timex_mode == 0) { // not timex mode
            wait(NULL);
        } else {               // timex mode
            wait_print_rusage(pid, args);
        }
        // printf("parent process\n");
    }
}

void exit_on_command(char** args) {
    if (strcmp(args[0], "exit") == 0 && args[1] == NULL) {
        exit(0);
    }
}

int is_timex_command(char** args) {
    if (strcmp(args[0], "timeX") == 0) {
        
        //remove the first element
        // printf("timex command\n");
        int n = get_args_length(args);
        // printf("n = %d\n", n);
        for (int i = 0; i < n; i++) {
            args[i] = args[i+1];
        }
        return 1;
    }
    return 0;
}

void wait_print_rusage(pid_t pid, char** args) {
    int status;
    struct rusage usage;
    wait4(pid, &status, 0, &usage);
    printf("(PID)%d  (CMD)%s    (user)%ld.%03ld s  (sys)%ld.%03ld s\n", pid, 
                                                                        args[0], 
                                                                        usage.ru_utime.tv_sec, 
                                                                        usage.ru_utime.tv_usec/1000, 
                                                                        usage.ru_stime.tv_sec, 
                                                                        usage.ru_stime.tv_usec/1000);


}

void print_error_message(char** args) {
    // perror message from args[0]
    char* error_message = malloc(1024 * sizeof(char));
    strcpy(error_message, "3230shell: '");
    strcat(error_message, args[0]);
    strcat(error_message, "'");    
    perror(error_message);
}