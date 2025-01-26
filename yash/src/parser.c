#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/execute.h"
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
  free(command);
}

void free_rel_process_container(rel_process_container *container) {
  if (container == NULL) return;
  free_parsed_struct(container->left_cmd);
  free_parsed_struct(container->right_cmd);
  free(container);
}

parsed* parse_command(int argc, char *argv[]) {
  // parse command for input file, output file, error file, and whether it's a background job
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
  char **argv = malloc(strlen(input) * sizeof(char *)); // command & argument array
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

  argv = realloc(argv, (argc) * sizeof(char *)); // shrink argv to fit the number of arguments
  argv[argc] = NULL; // set last element to NULL for execvp

  return argv;
}

rel_process_container* parse_by_pipe(char *input) {
 return NULL;
}

void parse(char *input) {
  //rel_process_container *rel_processes = parse_by_pipe();
  
  int input_size = 0;
  char ** parsed_input = parse_by_space(input);
  while (parsed_input[input_size] != NULL) {
    input_size++;
  }

  parsed *command = parse_command(input_size, parsed_input);
  
  execute_command(command->argc, command->argv);

  free_parsed_struct(command);
}
