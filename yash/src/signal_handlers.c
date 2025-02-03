#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <../include/execute.h>

void handle_SIGINT(int signal) {
  // Handle interrupt signal
  pid_t fg_pgid = tcgetpgrp(0);

  if (fg_pgid != getpgrp()) {
    // fg process group not equal to console's process group
    kill(-fg_pgid, SIGINT);
  } else {
    printf("\n# ");
  }
}

void handle_SIGTSTP(int signal) {
  // Handle suspend signal
  pid_t fg_pgid = tcgetpgrp(0);

  if (fg_pgid != getpgrp()) {
    // fg process group not equal to console's process group
    kill(-fg_pgid, SIGTSTP);
  } else {
    printf("\n# ");
  }
}

void handle_SIGQUIT(int signal) {
  // Handle quit signal
  pid_t fg_pgid = tcgetpgrp(0);
  kill(SIGTERM, fg_pgid);
  kill(SIGTERM, getpgrp());
}