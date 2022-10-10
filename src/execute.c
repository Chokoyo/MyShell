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

# define MAX_CHAR 1024

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

    // handle all built-in commands
    exit_on_command(args);
    int timex_mode = is_timex_command(args); // 0 if not timex, 1 if timex
    int background_mode = is_background_command(args, timex_mode); // 0 if not background, 1 if background
    if (background_mode == -1) {
        // error condition
        return;
    }

    // parse the instruction to an array of commands if involve pipe
    char ***all_cmd = parse_command(args, '|');
    int num_cmd = 0;
    while (all_cmd[num_cmd] != NULL) {
        num_cmd++;
    }
    
    // print all commands
    for (int i = 0; i < num_cmd; i++) {
        printf("command %d: ", i);
        for (int j = 0; all_cmd[i][j] != NULL; j++) {
            printf("%s ", all_cmd[i][j]);
        }
        printf("\n");
    }

    // check whether has empty command
    if (check_empty_command(all_cmd, num_cmd) == 1) {
        return;
    }
    
    // execute the commands
    int fds[num_cmd - 1][2];
    char* timex_info = malloc(MAX_CHAR * sizeof(char));
    for (int i = 0; i < num_cmd; i++) {
        pipe(fds[i]);
        pid_t pid = fork();
        if (pid == 0) {
            // child process
            while (!sigusr1_received);
            // if not the first command, redirect the input
            if (i != 0) {
                dup2(fds[i-1][0], STDIN_FILENO);
            }
            // if not the last command, redirect the output
            if (i != num_cmd - 1) {
                dup2(fds[i][1], STDOUT_FILENO);
            }
            // close all pipes
            close(fds[i-1][0]);
            close(fds[i][1]);

            // execute the command
            execvp(all_cmd[i][0], all_cmd[i]);

            // error handling
            print_error_message(args);
            exit(-1);
        } else {
            // parent process
            kill(pid, SIGUSR1);
            // wait for the child process to finish
            if (timex_mode == 0) { 
                // wait for all child processes to finish
                waitpid(pid, NULL, 0);
            } else {               
                // timex mode
                wait_store_rusage(pid, all_cmd[i], timex_info);
            }
            // close the unused file descriptors
            close(fds[i-1][0]);
            close(fds[i][1]);

            // if last command, print the timex info
            if (i == num_cmd - 1 && timex_mode == 1) {
                printf("%s", timex_info);
            }
        }
    }
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

int is_background_command(char** args, int timex_mode) {
    int i = get_args_length(args) - 1;
    int j = 0;
    while (args[i][j] != '\0') j++;
    j = j - 1;
    int m = 0;
    int n = 0;
    while (args[m] != NULL) {
        while (args[m][n] != '\0') {
            if (args[m][n] == '&' && (m != i || n != j)) {
                print_message(args, "'&' should not appear in the middle of the command line");
                return -1;
            }
            n++;
        }
        m++;
    }

    // find the last char args[i - 1][j - 1]
    // while (args[i] != NULL) i++;
    // while (args[i - 1][j] != '\0') j++;
    
    if (args[i][j] == '&') {
        args[i][j] = '\0';
        if (args[i][0] == '\0') {
            args[i] = NULL;
        }
        // check if have other "&" in the command
        if (timex_mode == 1) {
            print_message(args, "\"timeX\" cannot be run in background mode");
            return -1;
        }

        // print_message(args, "Background Mode Activated");
        return 1;
    }
    return 0;
}

int is_timex_command(char** args) {
    if (strcmp(args[0], "timeX") == 0 && args[1] != NULL) {
        int n = get_args_length(args);
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

int check_empty_command(char*** all_cmd, int num_cmd) {
    for (int i = 0; i < num_cmd; i++) {
        if (all_cmd[i][0] == NULL) {
            if (i == 0 || i == num_cmd - 1) {
                print_message(all_cmd[i], "Should not have the | sign as the first or last character");
            } else {
                print_message(all_cmd[i], "should not have two consecutive | without in-between command");
            }
            return 1;
        }
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

void wait_store_rusage(pid_t pid, char** args, char* timex_info) {
    char* string = malloc(MAX_CHAR * sizeof(char));
    int status = 0;
    struct rusage usage;
    // reset usage
    memset(&usage, 0, sizeof(usage));
    wait4(pid, &status, 0, &usage);
    sprintf(string, "(PID)%d  (CMD)%s    (user)%ld.%03ld s  (sys)%ld.%03ld s\n", pid, 
                                                                        args[0], 
                                                                        usage.ru_utime.tv_sec, 
                                                                        usage.ru_utime.tv_usec/1000, 
                                                                        usage.ru_stime.tv_sec, 
                                                                        usage.ru_stime.tv_usec/1000);
    strcat(timex_info, string);
    free(string);
}

void print_error_message(char** args) {
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "timeX") == 0) { // ommit built-in commands
        return;
    }
    char* err_msg = malloc(1024 * sizeof(char));
    sprintf(err_msg, "3230shell: '%s'", args[0]);
    perror(err_msg);
}

void print_message(char** args, char* message) {
    printf("3230shell: %s\n", message);
}
