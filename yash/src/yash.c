#include "../include/parser.h"
#include "../include/execute.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
/* TESTING COMMANDS

cat in1.txt > out1.txt | cat < in2.txt
mkdir "project folder" <-- just create it with the quotes
echo "hello world" <-- just print it with the quotes it's ok to ignore
wc < in1.txt | wc -l
wc <-- currently doesn't do anything

*/

int main() {

  char *cmdInput;

  while (cmdInput = readline("# ")) {
    // read yash console input
    if (strlen(cmdInput) == 0) {
      free(cmdInput);
      continue;
    } else if (strcmp(cmdInput, "exit") == 0) {
      free(cmdInput);
      exit(0);
    }

    parse(cmdInput);

    free(cmdInput);
  }
  return 0;
}