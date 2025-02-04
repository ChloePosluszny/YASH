#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
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
ping google.com &
*/
/*
TODO: Implement fg and bg commands
TODO: (Time permitting) reorder complete jobs list by job number
*/

void on_exit_function() {
  // terminate all processes
  int i = 0;
  while (jobs !=NULL && jobs[i] != NULL) {
    killpg(jobs[i]->pgid, SIGTERM);
    i++;
  }
  free_jobs_array(&jobs);
  free_jobs_array(&completed_bg_jobs);
}

int main() {
  atexit(on_exit_function);

  signal(SIGINT, handle_SIGINT);
  signal(SIGTSTP, handle_SIGTSTP);
  signal(SIGQUIT, handle_SIGQUIT);
  signal(SIGCHLD, handle_SIGCHLD);

  char *cmdInput;

  while (cmdInput = readline("# ")) {
    // main cmd terminal loop
    while (completed_bg_jobs != NULL && completed_bg_jobs[0] != NULL) {
      // print completed jobs on new line character
      job_container *job = pop_job(&completed_bg_jobs, true); 
      char *pos_or_neg = job->argc ? "+" : "-";

      printf("[%d]%s\t%s\t\t%s\n", 
              job->job_num, 
              pos_or_neg, 
              job->status, 
              job->cmd);
      free_job_container(job);
    }

    // handle empty command
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