#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>

typedef struct parsed {
  char **argv;
  int argc;
  char *input_file;
  char *output_file;
  char *error_file;
  bool is_bg_job;
} parsed;

typedef struct rel_process_container {
  parsed *left_cmd;
  parsed *right_cmd;
} rel_process_container;

void free_array_mem(char *array[], int size);

void free_parsed_struct(parsed *command);

void free_rel_process_container(rel_process_container *container);

parsed* parse_command(int argc, char *argv[]);

char** parse_by_space(char *input);

rel_process_container* parse_by_pipe(char *input);

void parse(char *input);

// void parse_pipe(int argc,char *argv[]);



#endif