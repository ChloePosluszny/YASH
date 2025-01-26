#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

bool has_pipe(int argc, char *argv[]) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "|") == 0) {
      return true;
    }
  }
  return false;
}

// int handle_pipe(int argc, char *argv[]) {
//   return 0;
// }

// int handle_redirect(int argc, char *argv[]) {
//   return 0;
// }

int execute_command(int argc, char *argv[]) {
  const int SUCCESS = 0;

  if (has_pipe(argc, argv)) {
    //handle_pipe(argc, argv);
  }

  int status;
  pid_t cpid;

  cpid = fork(); 
  if (cpid == 0) {
    execvp(argv[0], argv);
    exit(SUCCESS);
  }
 
  /* Parent reaps children exits */
  waitpid(-1,&status, 0); // wait for child to finish may have to edit the -1 and 0 in the future when dealing with bg processes
}

// int handle_pipe(int argc, char *argv[]) {
//   return 0;
// }

// int handle_redirect(int argc, char *argv[]) {
//   return 0;
// }
