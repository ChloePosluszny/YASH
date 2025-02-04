#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/common.h"
#include "../include/signal_handlers.h"
#include "../include/jobs.h"

void handle_pipe(rel_process_container *rel_processes, char *side, int *pipefd) {
  bool is_pipe = rel_processes->right_cmd != NULL ? true : false;
  if (is_pipe) {
    int read = 0;
    int write = 1;
    if (strcmp(side, "left") == 0) {
      close(pipefd[read]); // close unused read end of pipe

      dup2(pipefd[write], STDOUT_FILENO); // hijack output with pipe
    } else if (strcmp(side, "right") == 0) {
      close(pipefd[write]); // close unused write end of pipe
      
      dup2(pipefd[read], STDIN_FILENO);
    }
  }
}

void handle_redirect(parsed *command) {
  //STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO
  bool is_input_redir = command->input_file != NULL ? true : false;
  bool is_output_redir = command->output_file != NULL ? true : false;
  bool is_error_redir = command->error_file != NULL ? true : false;
  if (is_input_redir) {
    int fd = open(command->input_file, O_RDONLY);
    if (fd == -1) {
      exit(EXIT_FAILURE);
    }
    dup2(fd, STDIN_FILENO);
  }
  if (is_output_redir) {
    int fd = open(command->output_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if (fd == -1) {
      exit(EXIT_FAILURE);
    }

    dup2(fd, STDOUT_FILENO);
  }
  if (is_error_redir) {
    int fd = open(command->error_file, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH);
    if (fd == -1) {
      exit(EXIT_FAILURE);
    }

    dup2(fd, STDERR_FILENO);
  }
}

void give_console_ctrl(pid_t pgid) {
  // provide console ctrl to process group
  signal(SIGTTOU, SIG_IGN);

  tcsetpgrp(STDIN_FILENO, pgid);
  tcsetpgrp(STDOUT_FILENO, pgid);
  tcsetpgrp(STDERR_FILENO, pgid);
}

int execute_command(rel_process_container *rel_processes) {
  // execute entire command
  char **argv_left = rel_processes->left_cmd->argv;
  int argc_left = rel_processes->left_cmd->argc;
  char **argv_right = NULL;
  int argc_right = 0;
  char *cmd = rel_processes->cmd;

  if (is_jobs_cmd(argv_left)) {
    return 0;
  }

  bool is_pipe = rel_processes->right_cmd != NULL ? true : false;
  bool is_bg_job = rel_processes->left_cmd->is_bg_job;

  int pipefd[2];
  pipe(pipefd);

  if (is_pipe) {
    // pipe exists and input/output redirection does not override pipe
    argv_right = rel_processes->right_cmd->argv;
    argc_right = rel_processes->right_cmd->argc;
  }

  int status;
  pid_t ppid = getpid();
  pid_t lcpid;
  pid_t rcpid;

  // create child processes
  lcpid = fork(); 
  if (lcpid == 0) {
    // left child
    setpgid(0,0); // create new child process group

    if (!is_bg_job) {
      // give fg job control of terminal
      give_console_ctrl(getpgrp());
      signal(SIGCHLD, SIG_DFL);
    }

    signal(SIGINT, handle_SIGINT);
    signal(SIGTSTP, handle_SIGTSTP);
    signal(SIGQUIT, handle_SIGQUIT);

    handle_pipe(rel_processes, "left", pipefd);
    handle_redirect(rel_processes->left_cmd); // conflicting output/input with write/read ends of pipe favor file redirects
    
    execvp(argv_left[0], argv_left);
    exit(EXIT_FAILURE);
  }
  rcpid = fork();
  if (rcpid == 0) {
    // right child
    setpgid(0, lcpid); //join left child's process group

    if (!is_bg_job) {
      // give fg job control of terminal
      give_console_ctrl(getpgrp());
      signal(SIGCHLD, SIG_DFL);
    }

    signal(SIGINT, handle_SIGINT);
    signal(SIGTSTP, handle_SIGTSTP); // should also push fg process to jobs list
    signal(SIGQUIT, handle_SIGQUIT);
    
    handle_pipe(rel_processes, "right", pipefd);
    handle_redirect(rel_processes->right_cmd); // conflicting output/input with write/read ends of pipe favor file redirects
    
    execvp(argv_right[0], argv_right);
    exit(EXIT_FAILURE);
  }

  if (is_bg_job) {
    // restore terminal ctrl to parent
    give_console_ctrl(ppid);
    push_job(cmd, lcpid, argv_left, argc_left, "Running");
  }

  close(pipefd[0]);
  close(pipefd[1]);

  if (!is_bg_job) {
    // wait for fg processes
    waitpid(lcpid, &status, WUNTRACED);
    if (is_pipe) {
        waitpid(rcpid, &status, WUNTRACED);
    }
    if (WIFSTOPPED(status)) {
      // if child process recieves stop signal
      push_job(cmd, lcpid, argv_left, argc_left, "Stopped");
    }

    // give terminal ctrl back to console
    give_console_ctrl(ppid);
  } else {
    // background processes
    waitpid(lcpid, &status, WCONTINUED | WNOHANG);
    if (is_pipe) {
      waitpid(rcpid, &status, WCONTINUED | WNOHANG);
    }
  }
  return status;
}