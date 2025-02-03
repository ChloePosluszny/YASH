#ifndef PARSER_H
#define PARSER_H

#include <stdbool.h>
#include "common.h"

void free_array_mem(char *array[], int size);

void free_parsed_struct(parsed *command);

void free_rel_process_container(rel_process_container *container);

parsed* parse_command(char *argv[]);

char** parse_by_space(char *input);

rel_process_container* parse_input(char *input);

#endif