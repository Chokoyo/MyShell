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

// #define SIGUSR1_RECEIVED 0

int sigusr1_received = 0;

void sigusr1_handler(int sig) {
    if (sig == SIGUSR1) {
        sigusr1_received = 1;
    }
}

void execute(char** args) {
    // check if the instruction is empty
    if (args[0] == NULL) {
        return;
    }

    sigusr1_received = 0;
    signal(SIGUSR1, sigusr1_handler);
    sighandler_init();

    exit_on_command(args);
    bool timex_mode = is_timex_command(args); // 0 if not timex, 1 if timex
    

    execute_single(args, timex_mode);

}

void execute_single(char** args, int timex_mode) {

    
    pid_t pid = fork();
    if (pid == 0) {
        // child process
        // printf("child process\n");

        // pause until SIGUSR1 signal from parent
        while (!sigusr1_received);
        execvp(args[0], args);

        // error handling
        print_error_message(args);
        exit(-1);
    } else {
        // parent process

        // send SIGUSR1 signal to child
        kill(pid, SIGUSR1);
        // while (!SIGUSR1_RECEIVED);
        // if in timex mode, use wait4() to get rusage
        if (timex_mode == 0) { // not timex mode
            waitpid(pid, NULL, 0);
        } else {               // timex mode
            wait_print_rusage(pid, args);
        }
        // printf("parent process\n");
    }
}

void exit_on_command(char** args) {
    if (strcmp(args[0], "exit") == 0 && args[1] == NULL) {
        print_message(args, "Terminated");
        exit(0);
    } else if (strcmp(args[0], "exit") == 0 && args[1] != NULL) {
        print_message(args, "\"exit\" with other arguments!!!");
    }
}

bool is_timex_command(char** args) {
    if (strcmp(args[0], "timeX") == 0 && args[1] != NULL) {
        
        //remove the first element
        // printf("timex command\n");
        int n = get_args_length(args);
        // printf("n = %d\n", n);
        for (int i = 0; i < n; i++) {
            args[i] = args[i+1];
        }
        return 1;
    } else if (strcmp(args[0], "timeX") == 0 && args[1] == NULL) {
        print_message(args, "\"timeX\" cannot be a standalone command");
        return 0;
    }
    return 0;
}

void wait_print_rusage(pid_t pid, char** args) {
    int status;
    struct rusage usage;
    wait4(pid, &status, 0, &usage);
    printf("\n");
    printf("(PID)%d  (CMD)%s    (user)%ld.%03ld s  (sys)%ld.%03ld s\n", pid, 
                                                                        args[0], 
                                                                        usage.ru_utime.tv_sec, 
                                                                        usage.ru_utime.tv_usec/1000, 
                                                                        usage.ru_stime.tv_sec, 
                                                                        usage.ru_stime.tv_usec/1000);


}

void print_error_message(char** args) {
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "timeX") == 0) { // ommit built-in commands
        return;
    }
    char* err_msg = malloc(1024 * sizeof(char));
    // strcpy(err_msg, "3230shell: '");
    // strcat(err_msg, args[0]);
    // strcat(err_msg, "'");
    sprintf(err_msg, "3230shell: '%s'", args[0]);
    perror(err_msg);
    // if (err_msg != NULL) {
    //     free(err_msg);
    // }
}

void print_message(char** args, char* message) {
    printf("3230shell: %s\n", message);
}
