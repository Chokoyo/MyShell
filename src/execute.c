// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – all requirements are implemented, including bonus

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h> 
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <setjmp.h>

#include "sllist.h"
#include "execute.h"
#include "sighandler.h"
#include "parser.h"

# define MAX_CHAR 1024
# define MAX_BACKGROUND 20

int sigusr1_received = 0;
char *background_info = NULL;
bool wait_flag = false;
extern SLList background_list;
extern bool print_background_info;
extern bool print_prompt;

struct rusage usage;

void sigusr1_handler(int sig) {
    if (sig == SIGUSR1) {
        sigusr1_received = 1;
    }
}

void sigchld_handler(int sig) {
    if (sig == SIGCHLD) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
        // pid_t pid = wait4(-1, &status, WNOHANG, &usage);
        int index = findNode(&background_list, (int) pid); // check if the pid is in the background list
        if (pid > 0 &&  index!= -1) {
            char *cmd_name = (getNode(&background_list, index))->command;
            if (WIFEXITED(status)) {
                char *info = malloc(MAX_CHAR);
                sprintf(info, "[%d] %s Done\n", pid, get_cmd_name(cmd_name));
                strcat(background_info, info);
                free(info);
            } else if (WIFSIGNALED(status)) {
                char *info = malloc(MAX_CHAR);
                sprintf(info, "[%d] %s Terminated\n", pid, get_cmd_name(cmd_name));
                strcat(background_info, info);
                free(info);
            }
            removeNode(&background_list, pid);
            print_background_info = false;
            wait_flag = true;
            print_prompt = false;
        }
    }
    signal(SIGCHLD, sigchld_handler);
}

void execute(char** args) {
    int background_mode = 0;
    int timex_mode = 0;

    // Initialize background information
    if (background_info == NULL) {
        background_info = malloc(MAX_CHAR);
        strcpy(background_info, "");
    }

    // Check if the instruction is empty
    if (args[0] == NULL) {
        if (strcmp(background_info, "") != 0 && print_background_info) {
            printf("%s", background_info);
            strcpy(background_info, "");
        }  
        wait_flag = false;
        return;
    }

    sigusr1_received = 0;
    signal(SIGUSR1, sigusr1_handler);
    sighandler_init();

    // handle all built-in commands
    exit_on_command(args);
    timex_mode = is_timex_command(args); // 0 if not timex, 1 if timex
    background_mode = is_background_command(args, timex_mode); // 0 if not background, 1 if background
    if (background_mode == -1) {
        // error condition
        return;
    }

    // parse the instruction to an array of commands if involve pipe
    char*** all_cmd = parse_command(args, '|');
    int num_cmd = 0;
    while (all_cmd[num_cmd] != NULL) {
        num_cmd++;
    }
    
    // print all commands
    // printf("background_mode: %d, timex_mode: %d, num_cmd: %d\n", background_mode, timex_mode, num_cmd);
    // for (int i = 0; i < num_cmd; i++) {
    //     printf("command %d: ", i);
    //     for (int j = 0; all_cmd[i][j] != NULL; j++) {
    //         printf("%s ", all_cmd[i][j]);
    //     }
    //     printf("\n");
    // }

    // Check whether has empty command
    if (check_empty_command(all_cmd, num_cmd) == 1) {
        return;
    }
    
    // Sigchld Initialize
    signal(SIGCHLD, sigchld_handler);

    // Create Variables for Execution
    int fds[num_cmd - 1][2];
    char* timex_info = malloc(MAX_CHAR * sizeof(char));
    strcpy(timex_info, "");
    Job *jobs[MAX_BACKGROUND]; // store all background jobs in the pipeline for one instruction

    // Start Execution
    for (int i = 0; i < num_cmd; i++) {
        pipe(fds[i]);
        pid_t pid = fork();
        if (pid == 0) {
            // Child process
            if (background_mode == 1) {
                setpgid(0, 0);
            }
            // Wait for the parent to send SIGUSR1
            if (i == 0) {
                while (!sigusr1_received);
            }
            // If not the first command, redirect the input
            if (i != 0) {
                dup2(fds[i-1][0], STDIN_FILENO);
            }
            // If not the last command, redirect the output
            if (i != num_cmd - 1) {
                dup2(fds[i][1], STDOUT_FILENO);
            }
            close(fds[i-1][0]);
            close(fds[i-1][1]);
            close(fds[i][1]);
            close(fds[i][0]);

            // Execute the command
            execvp(all_cmd[i][0], all_cmd[i]);
            
            // Error handling
            print_error_message(all_cmd[i]);
            exit(-1);
        } else {
            // Parent process
            if (i==0) kill(pid, SIGUSR1);

            if (background_mode == 0){
                // Wait for the child process to finish
                if (timex_mode == 0) { 
                    // wait for all child processes to finish
                    waitpid(pid, NULL, 0);
                } else if(timex_mode == 1) {               
                    // timex mode

                    wait_store_rusage(pid, all_cmd[i], timex_info);
                }
            
            } else {
                // Record the background process info
                jobs[i] = malloc(sizeof(Job)); //todo: support pipe
                jobs[i]->pid = pid;
                jobs[i]->command = malloc(MAX_CHAR);
                memset(jobs[i]->command, 0, MAX_CHAR);
                strcpy(jobs[i]->command, all_cmd[i][0]);
                insertNode(&background_list, jobs[i]);
                waitpid(pid, NULL, WNOHANG);
            }
            // if not the first command, close the input
            if (i != 0) {
                close(fds[i-1][0]);
            }

            // if not the last command, close the output
            if (i != num_cmd - 1) {
                close(fds[i][1]);
            }

        }
    }
    // Execution complete

    // Close the unused file descriptors
    for (int i = 0; i < num_cmd - 1; i++) {
        close(fds[i][0]);
        close(fds[i][1]);
    }

    if (wait_flag) {
        wait_flag = false;
        wait(NULL);
    }

    // Print timex info
    if (timex_mode == 1) {
        printf("%s", timex_info);
        memset(timex_info, 0, MAX_CHAR);
    }

    // Print background info
    if (strcmp(background_info, "") != 0) {

        printf("%s", background_info);
        strcpy(background_info, "");
    }

    // Free memory
    free(timex_info);

    if (background_mode == 0) {
        print_prompt = true;
    }

}

void exit_on_command(char** args) {
    if (args[0] == NULL) {
        return;
    }
    if (strcmp(args[0], "exit") == 0 && args[1] == NULL) {
        // Terminate all background processes
        Job *job = background_list.sentinel->next;
        while (job != background_list.sentinel) {
            kill(job->pid, SIGKILL);
            job = job->next;
        }
        // Free memory
        deleteSLList(&background_list);
        free(args);
        free(background_info);
        print_message(args, "Terminated");
        exit(0);
    } else if (strcmp(args[0], "exit") == 0 && args[1] != NULL) {
        print_message(args, "\"exit\" with other arguments!!!");
    }
}

int is_background_command(char** args, int timex_mode) {
    int i = 0;
    int j = 0;
    int flag = 0;
    for (i = 0; args[i] != NULL; i++) {
        for (j = 0; args[i][j] != '\0'; j++) {
            if (args[i][j] == '&') {
                flag = 1;
                break;
            }
        }
        if (flag == 1) {
            break;
        }
    }
    
    if (args[i] != NULL) {
        if (args[i][j + 1] != '\0' || args[i + 1] != NULL) {
            print_message(args, "'&' should not appear in the middle of the command line");
            return -1;
        }
    }

    while (args[i] != NULL) i++;
    while (args[i - 1][j] != '\0') j++;
    
    if (args[i - 1][j - 1] == '&') {
        args[i - 1][j - 1] = '\0';
        if (args[i - 1][0] == '\0') {
            args[i - 1] = NULL;
        }
        // check if have other "&" in the command
        if (timex_mode == 1) {
            print_message(args, "\"timeX\" cannot be run in background mode");
            return -1; // error
        }
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
                print_message(all_cmd[i], "should not have the | sign as the first or last character");
            } else {
                print_message(all_cmd[i], "should not have two consecutive | without in-between command");
            }
            return 1;
        }
    }
    return 0;
}


void wait_store_rusage(pid_t pid, char** args, char* timex_info) {
    char* string = malloc(MAX_CHAR * sizeof(char));
    int status = 0;
    // reset usage
    memset(&usage, 0, sizeof(usage));
    wait4(pid, &status, 0, &usage);
    // wait twice incase interrupted by sigchld
    wait4(pid, &status, 0, &usage);
    sprintf(string, "(PID)%d  (CMD)%s    (user)%ld.%03ld s  (sys)%ld.%03ld s\n", 
                                                                pid, 
                                                                get_cmd_name(args[0]), 
                                                                usage.ru_utime.tv_sec, 
                                                                usage.ru_utime.tv_usec/1000,
                                                                usage.ru_stime.tv_sec,
                                                                usage.ru_stime.tv_usec/1000);
                                                                        
    strcat(timex_info, string);
    free(string);
}

void print_error_message(char** args) {
    // Ommit built-in commands
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "timeX") == 0) { 
        return;
    }
    char* err_msg = malloc(1024 * sizeof(char));
    sprintf(err_msg, "3230shell: '%s'", args[0]);
    perror(err_msg);
    free(err_msg);
}

void print_message(char** args, char* message) {
    printf("3230shell: %s\n", message);
}
