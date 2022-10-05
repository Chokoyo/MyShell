#ifndef EXECUTE_H
#define EXECUTE_H

void execute(char** args);
void execute_single(char** args, int timex_mode);
void exit_on_command(char** args);
int is_timex_command(char** args);
void wait_print_rusage(pid_t pid, char** args);
void print_error_message(char** args);


#endif