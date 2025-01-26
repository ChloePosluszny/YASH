#ifndef EXECUTE_H
#define EXECUTE_H

#include "common.h"

// int handle_pipe(int argc, char *argv[])

void handle_redirect(parsed *command);

int execute_command(rel_process_container *rel_processes);

#endif