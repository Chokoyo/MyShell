#include <stdbool.h>

#ifndef SIGHANDLER_H
#define SIGHANDLER_H

void sighandler_init();
void sigterm_handler();
void sigkill_handler(int sig);
void sighup_handler(int sig);
void sigint_handler(int sig);
void sigint_main_handler(int sig);
void sigquit_handler(int sig);
void sigusr1_handler(int sig);
void sigchld_handler(int sig);
void sigint_main_init(int sig);

#endif