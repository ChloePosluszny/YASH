#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include "../include/parser.h"
#include "../include/signal_handlers.h"
/* TESTING COMMANDS

cat in1.txt > out1.txt | cat < in2.txt
mkdir "project folder" <-- just create it with the quotes
echo "hello world" <-- just print it with the quotes it's ok to ignore
wc < in1.txt | wc -l
some_command 2> error_log.txt
ls | wc
cat < temp.txt > output.txt 2> error.txt
cat < in1.txt > out1.txt | cat < in2.txt > out2.txt <-- seems like output redirects hijack the write end of the pipe and input redirects hijack the read end of the pipe
echo "hello world" > temp.txt
cat -E < in1.txt > out1.txt
wc < out.txt | sleep 4 <-- currently broken (realloc invalid pointer)
*/

//TODO: MAKE ALL CHILD PROCESSES DIE ON EXIT

// void on_exit() {
//   // terminate all processes
  
// }

int main() {
  // atexit(on_exit);

  signal(SIGINT, handle_SIGINT);
  signal(SIGTSTP, handle_SIGTSTP);
  signal(SIGQUIT, handle_SIGQUIT);

  char *cmdInput;

  while (cmdInput = readline("# ")) {
    // read yash console input
    if (strlen(cmdInput) == 0) {
      free(cmdInput);
      continue;
    }

    parse(cmdInput);

    free(cmdInput);
  }
  return 0;
}