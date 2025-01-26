#ifndef COMMON_H
#define COMMON_H

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

#endif