#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/execute.h"
#include "../include/common.h"
#include <stdbool.h>

void free_array_mem(char *array[], int size) {
  for (int i = 0; i < size; i++) {
    free(array[i]);
  }
}

void free_parsed_struct(parsed *command) {
  if (command == NULL) return;
  free_array_mem(command->argv, command->argc);
  free(command->input_file);
  free(command->output_file);
  free(command->error_file);
}

void free_rel_process_container(rel_process_container *container) {
  if (container == NULL) return;
  free_parsed_struct(container->left_cmd);
  free_parsed_struct(container->right_cmd);
  free(container->cmd);
}

parsed* parse_command(char *argv[]) {
  // parse command for input file, output file, error file, and whether it's a background job
  int argc = 0;
  while (argv[argc] != NULL) {
    argc++;
  }

  char *input_file = NULL;
  char *output_file = NULL;
  char *error_file = NULL;
  bool is_bg_job = false;
  int end_of_command = argc;

  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "<") == 0) {
      // input file
      if (i < end_of_command) {
        end_of_command = i;
      }
      if (i+1 != argc) {
        input_file = strdup(argv[i + 1]);
      }
    } else if (strcmp(argv[i], ">") == 0) {
      // output file
      if (i < end_of_command) {
        end_of_command = i;
      }
      if (i+1 != argc) {
        output_file = strdup(argv[i + 1]);
      }
    } else if (strcmp(argv[i], "2>") == 0) {
      // error file
      if (i < end_of_command) {
        end_of_command = i;
      }
      if (i+1 != argc) {
        error_file = strdup(argv[i + 1]);
      }
    } else if (strcmp(argv[i], "&") == 0) {
      // bg job
      if (i < end_of_command) {
        end_of_command = i;
      }
      if (argv[i+1] == NULL) {
        is_bg_job = true;
      }
    }
  }

  argv[end_of_command] = NULL;
  argc = end_of_command;

  parsed *command = malloc(sizeof(parsed));
  if (command == NULL) {
    exit(EXIT_FAILURE);
  }
  command->argv = argv;
  command->argc = argc;
  command->input_file = input_file;
  command->output_file = output_file;
  command->error_file = error_file;
  command->is_bg_job = is_bg_job;

  return command;
}

char** parse_by_space(char *input) {
  // parse command by spaces
  char **argv = malloc((strlen(input)+1) * sizeof(char *)); // command & argument array
  if (argv == NULL) {
    exit(EXIT_FAILURE);
  }
  int argc = 0; // argument count

  const char DELIM[] = " "; // seperate by spaces
  char *token_save_ptr;
  char *token = strtok_r(input, DELIM, &token_save_ptr);
  
  while (token != NULL) {
    // print each word of the command separated by spaces
    argv[argc] = malloc((strlen(token)+1) * sizeof(char));
    if (argv[argc] == NULL) {
      exit(EXIT_FAILURE);
    }
    strcpy(argv[argc], token);
    argc++;

    token = strtok_r(NULL, DELIM, &token_save_ptr);
  }
  argv = realloc(argv, (argc + 1) * sizeof(char *)); // shrink argv to fit the number of arguments
  if (argv == NULL) {
    exit(EXIT_FAILURE);
  }

  argv[argc] = NULL; // set last element to NULL

  return argv;
}

rel_process_container* parse_input(char *input) {
  // Build container to hold the left and right commands' data
  rel_process_container *container = malloc(sizeof(rel_process_container));

  container->cmd = strdup(input);

  if (container == NULL) {
    exit(EXIT_FAILURE);
  }

  char PIPE_DELIM[] = "|";
  char *token_save_ptr;
  char *token = strtok_r(input, PIPE_DELIM, &token_save_ptr);
  
  // parse for commands on left and right side of pipe
  char *left_cmd = token;
  token = strtok_r(NULL, PIPE_DELIM, &token_save_ptr);
  char *right_cmd = token;
  
  if (right_cmd!=NULL) {
    container->left_cmd = parse_command(parse_by_space(left_cmd));
    container->right_cmd = parse_command(parse_by_space(right_cmd));
  } else {
    container->left_cmd = parse_command(parse_by_space(left_cmd));
    container->right_cmd = NULL;
  }

  return container;
}
