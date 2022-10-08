#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#include "sighandler.h"


void sighandler_init() {
    signal(SIGHUP, sighup_handler);
    signal(SIGINT, sigint_handler);
    signal(SIGQUIT, sigquit_handler);
}

void sigterm_handler(int sig) {
    printf("Terminated\n");
    signal(SIGTERM, SIG_DFL);
}

void sighup_handler(int sign) {
    printf("Hangup\n");
    signal(SIGHUP, SIG_DFL);
}

void sigint_handler(int sig) {
    printf("Interrupt\n");
    signal(SIGINT, SIG_DFL);
}

void sigkill_handler(int sig) {
    printf("Killed\n");
    signal(SIGKILL, SIG_DFL);
}

void sigquit_handler(int sig) {
    printf("Quited\n");
    signal(SIGQUIT, SIG_DFL);
}

// void sigusr1_handler(int sig) {
//     #define SIGUSR1_RECEIVED 1
// }

void sigchld_handler(int sig) {
    // print which signal terminate the child process
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
