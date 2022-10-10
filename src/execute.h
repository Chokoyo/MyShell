#include <stdbool.h>

#ifndef EXECUTE_H
#define EXECUTE_H

void execute(char** args);
void execute_single(char** args, int timex_mode);
void exit_on_command(char** args);
int is_background_command(char** args, int timex_mode);
int is_timex_command(char** args);
int check_empty_command(char*** all_cmd, int num_cmd);
void wait_print_rusage(pid_t pid, char** args);
void wait_store_rusage(pid_t pid, char** args, char* rusage_str);
void print_error_message(char** args);
void print_message(char** args, char* message);


#endif