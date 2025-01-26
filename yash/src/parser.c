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

//cat -E < in1.txt > out1.txt
parsed* parse_redirect_and_bg(int argc, char *argv[]) {
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
  
  for (int i = 0; i < argc; i++) {
    printf("argv[%d]: %s\n", i, argv[i]);
  }
  printf("Input: %s, Output: %s, Error: %s, Background: %d\n",
         input_file ? input_file : "NULL",
         output_file ? output_file : "NULL",
         error_file ? error_file : "NULL",
         is_bg_job);

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

  return command; // TODO FREE EVERYTHING IN PARSED WHEN DONE WITH THE COMMAND
}

void parse(char *input) {
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
  argv[argc] = NULL; // set last element of argv is NULL
  
  parsed *command = parse_redirect_and_bg(argc, argv);
  
  execute_command(command->argc, command->argv);
  
  free_parsed_struct(command);
}

// void parse_pipe(int argc, char *argv[]) {

// }

// void parse_redirect(int argc,char *argv[]) {

// }