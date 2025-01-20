#include <string.h>
#include <stdbool.h>
#include <stdio.h>

bool parse(char *input) {
  char *tokenizedStr = strtok(input, " "); // grab first word

  while (tokenizedStr != NULL) {
    //print each word
    printf("%s\n", tokenizedStr);
    tokenizedStr = strtok(NULL, " ");
  }
  return false;
}

//def isValidCmd

//def formatCmd
/* remove \n from input by replacing with null \0 */