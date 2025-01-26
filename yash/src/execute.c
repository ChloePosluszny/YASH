#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/common.h"

// int handle_pipe(int argc, char *argv[]) {
//   return 0;
// }

// int handle_redirect(int argc, char *argv[]) {
//   return 0;
// }

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

int execute_command(rel_process_container *rel_processes) {
  //S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH
  char **argv_left = rel_processes->left_cmd->argv;
  int argc_left = rel_processes->left_cmd->argc;
  char **argv_right = NULL;
  int argc_right = 0;

  bool is_pipe = rel_processes->right_cmd != NULL ? true : false;
  bool is_bg_job = rel_processes->left_cmd->is_bg_job;

  int pipefd[2];

  if (is_pipe) {
    argv_right = rel_processes->right_cmd->argv;
    argc_right = rel_processes->right_cmd->argc;
  }

  int status;
  pid_t cpid;

  cpid = fork(); 
  if (cpid == 0) {
    // left child
    handle_redirect(rel_processes->left_cmd);
    execvp(argv_left[0], argv_left);
    exit(EXIT_SUCCESS);
  }
  waitpid(-1,&status, 0); // wait for child to finish may have to edit the -1 and 0 in the future when dealing with bg processes
}