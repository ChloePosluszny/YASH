#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdbool.h>
#include "common.h"

bool has_pipe(int argc, char *argv[]);

// int handle_pipe(int argc, char *argv[])

// int handle_redirect(int argc, char *argv[])

int execute_command(rel_process_container *rel_processes);

#endif