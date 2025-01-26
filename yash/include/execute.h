#ifndef EXECUTE_H
#define EXECUTE_H
#include <stdbool.h>

bool has_pipe(int argc, char *argv[]);

// int handle_pipe(int argc, char *argv[])

// int handle_redirect(int argc, char *argv[])

int execute_command(int argc, char *argv[]);



#endif