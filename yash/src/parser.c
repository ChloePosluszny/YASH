#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/execute.h"

void free_array_mem(char *array[], int size) {
  for (int i = 0; i < size; i++) {
    free(array[i]);
  }
}

void parse(char *input) {
  char **argv = malloc(strlen(input) * sizeof(char *));; // command & argument array
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
  execute_command(argc, argv);
  free_array_mem(argv, argc);
}

// void parse_pipe(int argc, char *argv[]) {

// }

// void parse_redirect(int argc,char *argv[]) {

// }