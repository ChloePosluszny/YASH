#include "../include/parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <stdbool.h>

int main() {

  char *cmdInput;

  while (cmdInput = readline("@ ")) {
    // read yash console input
    parse(cmdInput);

    if (strcmp(cmdInput, "exit") == 0) {
      free(cmdInput);
      exit(0);
    }

    free(cmdInput);
  }
  return 0;
}