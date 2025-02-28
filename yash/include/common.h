#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>
#include <sys/types.h>

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
  char *cmd;
} rel_process_container;

typedef struct job_container {
  char *cmd;
  pid_t pgid;
  char **argv;
  int argc;
  int job_num;
  char *status; // Running, Stopped, Done
} job_container;

#endif