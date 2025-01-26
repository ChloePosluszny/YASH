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

parsed parse_redirect_and_bg(int argc, char *argv[]);

void parse(char *input);

// void parse_pipe(int argc,char *argv[]);



#endif