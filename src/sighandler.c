// Student name and No.: Gu Zhuangcheng, 3035827110
// Development platform: c3230-m1-ubuntu docker image
// Remark – all requirements are implemented, including bonus

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <setjmp.h>
#include <sys/wait.h>
#include <string.h>
#include "sighandler.h"
#include "sllist.h"
#include "parser.h"

# define MAX_CHAR 1024
# define MAX_BACKGROUND 20

extern bool print_prompt;
extern int sigusr1_received;
extern SLList background_list;
extern bool print_background_info;
extern char *background_info;
extern bool wait_flag;
extern bool print_prompt;
extern int term_sig;

void sighandler_init() {
    signal(SIGHUP, sighup_handler);
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
    signal(SIGKILL, sigkill_handler);
    signal(SIGTERM, sigterm_handler);
    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCHLD, sigchld_handler);
}

void sigterm_handler(int sig) {
    print_prompt = false;
    term_sig = 1;
    signal(SIGTERM, sigterm_handler);
}

void sighup_handler(int sign) {
    print_prompt = false;
    term_sig = 1;
    signal(SIGHUP, sighup_handler);
}

void sigint_handler(int sig) {
    print_prompt = false;
    term_sig = 1;
    signal(SIGINT, sigint_handler);
}

void sigkill_handler(int sig) {
    print_prompt = false;
    term_sig = 1;
    signal(SIGKILL, sigkill_handler);
}

void sigquit_handler(int sig) {
    print_prompt = false;
    term_sig = 1;
    signal(SIGQUIT, sigquit_handler);
}

void sigusr1_handler(int sig) {
    if (sig == SIGUSR1) {
        sigusr1_received = 1;
    }
}

void sigchld_handler(int sig) {
    if (sig == SIGCHLD) {
        int status;
        pid_t pid = waitpid(-1, &status, WNOHANG);
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
        }
        print_prompt = false; // will recover at end of execution if not background
    }
    signal(SIGCHLD, sigchld_handler);
}

void sigint_main_handler(int sig) {
    printf("\n");
    signal(SIGINT, sigint_handler);
    fflush(stdin);
}

void sigint_main_init(int sig) {
    signal(SIGINT, sigint_main_handler);
}
