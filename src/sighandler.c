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

void sighup_handler(int sign) {
    printf("SIGHUP received, pid = %d\n", getpid());
    signal(SIGHUP, sighup_handler);
}

void sigint_handler(int sig) {
    printf("SIGINT received, pid = %d\n", getpid());
    signal(SIGINT, sigint_handler);
}

void sigquit_handler(int sig) {
    printf("SIGQUIT received, pid = %d\n", getpid());
    signal(SIGQUIT, sigquit_handler);
}

