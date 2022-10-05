#ifndef SIGHANDLER_H
#define SIGHANDLER_H

void sighandler_init();

void sighup_handler(int sig);
void sigint_handler(int sig);
void sigquit_handler(int sig);


#endif