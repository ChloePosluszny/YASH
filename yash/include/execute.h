#ifndef EXECUTE_H
#define EXECUTE_H

#include "common.h"

void handle_pipe(rel_process_container *rel_processes, char *side, int *pipefd);

void handle_redirect(parsed *command);

void give_console_ctrl(pid_t pgroup);

int execute_command(rel_process_container *rel_processes);

#endif