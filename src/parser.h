#ifndef PARSER_H
#define PARSER_H

char **parse(char *input);
int get_args_length(char** args);
char* get_cmd_name(char* args);
char* read_input();
char ***parse_command(char **instr, char delimiter);

#endif