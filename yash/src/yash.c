#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <readline/readline.h>
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/common.h"
#include "../include/signal_handlers.h"
#include "../include/jobs.h"
/* TESTING COMMANDS

cat in1.txt > out1.txt | cat < in2.txt
mkdir "project folder" <-- just create it with the quotes
echo "hello world" <-- just print it with the quotes it's ok to ignore
wc < in1.txt | wc -l
some_command 2> error_log.txt
ls | wc
cat < temp.txt > output.txt 2> error.txt
cat < in1.txt > out1.txt | cat < in2.txt > out2.txt
echo "hello world" > temp.txt
cat -E < in1.txt > out1.txt
wc < out.txt | sleep 4 <-- was broken, now it's fixed (realloc invalid pointer)
ls > out.txt | sleep 2
*/

void on_exit_function() {
  // terminate all processes
  int i = 0;
  while (jobs !=NULL && jobs[i] != NULL) {
    killpg(jobs[i]->pgid, SIGTERM);
    i++;
  }
  free_jobs_array();
}

int main() {
  atexit(on_exit_function);

  signal(SIGINT, handle_SIGINT);
  signal(SIGTSTP, handle_SIGTSTP);
  signal(SIGQUIT, handle_SIGQUIT);
  //signal(SIGCHLD, handle_SIGCHLD);

  char *cmdInput;

  while (cmdInput = readline("# ")) {
    // read yash console input
    if (strlen(cmdInput) == 0) {
      free(cmdInput);
      continue;
    }
    
    rel_process_container *rel_processes = parse_input(cmdInput);

    execute_command(rel_processes);

    free_rel_process_container(rel_processes);

    free(cmdInput);
  }
  return 0;
}