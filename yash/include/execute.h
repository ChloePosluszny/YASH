#ifndef EXECUTE_H
#define EXECUTE_H

#include "common.h"

void handle_pipe(rel_process_container *rel_processes, char *side, int *pipefd);

void handle_redirect(parsed *command);

int execute_command(rel_process_container *rel_processes);

#endif