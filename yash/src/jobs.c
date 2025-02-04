#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/common.h"
#include "../include/parser.h"
#include "../include/execute.h"
#include "../include/signal_handlers.h"

job_container **jobs = NULL; // Job container type has elements char **argv, int argc, int job_num, and char *status
job_container **completed_bg_jobs = NULL;

void free_job_container(job_container *job) {
  if (job == NULL) return;

  if (job->argv != NULL) {
    free_array_mem(job->argv, job->argc);
  }
  
  free(job->status);
  free(job->cmd);
}

void free_jobs_array(job_container ***jobs_array) {
  if (*jobs_array != NULL) {
    int i = 0;
    while ((*jobs_array)[i] != NULL) {
      free_job_container((*jobs_array)[i]);
      i++;
    }
  }
}

char **deep_copy_array(char *array[], int size) {
  char **copy = malloc(size * sizeof(char*));
  
  for (int i = 0; i < size; i++) {
    copy[i] = strdup(array[i]);
  }

  return copy;
}

job_container* deep_copy_job(job_container *job) {
  if (job != NULL) {
  job_container *job_copy = malloc(sizeof(job_container));
  
  job_copy->cmd = strdup(job->cmd);
  job_copy->pgid = job->pgid;
  job_copy->argv = deep_copy_array(job->argv, job->argc);
  job_copy->argc = job->argc;
  job_copy->status = strdup(job->status);
  job_copy->job_num = job->job_num;
  
  return job_copy;
  } else {
    return NULL;
  }
}

void remove_job(job_container *job) {
  if (jobs == NULL || jobs[0] == NULL) return;

  // find the job and index
  int i = 0;
  int size = 0;
  while (jobs[size] != NULL) {
    if (jobs[size]->pgid == job->pgid) {
      i = size;
    }
    size++;
  }

  // free the removed job
  free_job_container(jobs[i]);

  // shift jobs left
  for (int j = i; j < size-1; j++) {
    jobs[j] = jobs[j + 1];
  }
  jobs[size - 1] = NULL;

  // shrink jobs array to fit
  jobs = realloc(jobs, size * sizeof(job_container *));
}

job_container* peek_job() {
  //read top of jobs stack/list
  if (jobs == NULL || jobs[0] == NULL) return NULL;

  int i = 0;
  while (jobs[i] != NULL) {
    if (jobs[i+1] == NULL) return jobs[i];
    i++;
  }
}

void push_job(job_container ***jobs_list, char *cmd, pid_t pgid, char *argv[], int argc, int job_num, char *status) {
  job_container *job = malloc(sizeof(job_container));
  job->cmd = strdup(cmd);
  job->pgid = pgid;
  job->argv = deep_copy_array(argv, argc);
  job->argc = argc;
  job->status = strdup(status);
  if (job_num == -1){
    job->job_num = peek_job()!=NULL ? peek_job()->job_num + 1 : 1;
  } else {
    job->job_num = job_num;
  }
  
  int size;
  if (*jobs_list == NULL || (*jobs_list[0]) == NULL) {
    size = 2;
  } else {
    int i = 0;
    while ((*jobs_list)[i] != NULL) {
      i++;
    }
    size = i+2;
  }

  *jobs_list = realloc(*jobs_list, (size)  * sizeof(job_container *));
  (*jobs_list)[size-1] = NULL;
  
  (*jobs_list)[size-2] = job;
}

job_container* pop_job(job_container ***jobs_list, bool is_FIFO) {
  if (*jobs_list == NULL || (*jobs_list)[0] == NULL) return NULL;

  job_container *job;
  int i = 0;

  if (is_FIFO) {
    if ((*jobs_list)[0] != NULL) {
      job = deep_copy_job((*jobs_list)[0]);
      free_job_container((*jobs_list)[0]);
      while ((*jobs_list)[i+1] != NULL) {
        (*jobs_list)[i] = (*jobs_list)[i+1];
        i++;
      }
      (*jobs_list)[i] = NULL;
    }

  } else {
    while ((*jobs_list)[i] != NULL) {
      if ((*jobs_list)[i+1] == NULL) {
        job = deep_copy_job((*jobs_list)[i]);
        free_job_container((*jobs_list)[i]);
        (*jobs_list)[i] = NULL;
      }
      i++;
    }
    i-=1;
  }
  *jobs_list = realloc(*jobs_list, (i) * sizeof(job_container *));
  return job;
}

void jobs_cmd() {
  int i = 0;
  while (jobs != NULL && jobs[i] != NULL) {
    // iterate through jobs array
    char *pos_or_neg = jobs[i+1] == NULL ? "+" : "-";

    printf("[%d]%s\t%s\t\t%s\n", 
            jobs[i]->job_num, 
            pos_or_neg, 
            jobs[i]->status, 
            jobs[i]->cmd);
    i++;
  }
}

void bg() {
  // handle bg command
  if (jobs == NULL || jobs[0] == NULL) return;
  
  job_container *job = NULL;
  int i = 0;
  while (jobs[i] != NULL) {
    // find most recent stopped job
    if (strcmp(jobs[i]->status, "Stopped") == 0) {
      job = jobs[i];
    }
    i++;
  }
  if (job == NULL) return; // if no job with status Stopped
  
  signal(SIGCHLD, handle_SIGCHLD);
  free(job->status);
  job->status = strdup("Running");
  kill (-(job->pgid), SIGCONT);
}

void fg() {
  // handle fg command
  job_container *job;
  if ((job = pop_job(&jobs ,false)) == NULL) return; //perform LIFO pop on jobs list to grab last job submitted to bg

  give_console_ctrl(job->pgid);
  kill (-(job->pgid), SIGCONT); // send sigcont signal to process group from top of jobs list 'stack'

  // wait for process to exit/terminate or stop
  int status;
  waitpid((job->pgid), &status, WUNTRACED);

  give_console_ctrl(getpgrp());

  if (WIFSTOPPED(status)) {
    // If the job was stopped, put it back in the jobs list
    push_job(&jobs, job->cmd, job->pgid, job->argv, job->argc, job->job_num, "Stopped");
  } else {
    // If the job completed, free the job container
    free_job_container(job);
  }
}

bool is_jobs_cmd(char *argv[]) {
  //check if command is a valid jobs cmd (i.e., if argv[0] is fg, bg, or jobs)
  char *cmd = argv[0];
  
  if (argv[1] != NULL) return false;
  
  if (strcmp(cmd, "jobs") == 0) {
    // jobs command sent
    jobs_cmd();
  } else if (strcmp(cmd, "fg") == 0) {
    // fg command sent
    fg();
  } else if (strcmp(cmd, "bg") == 0) {
    // bg command sent
    bg();
  } else {
    return false;
  }
  return true;
}

void check_bg_jobs() {
  // on child exit check to see if it's in bg jobs
  int status;
  pid_t pid;

  give_console_ctrl(getpgrp());

  while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
    int i = 0;
    while (jobs !=NULL && jobs[i] != NULL) {
      if (jobs[i]->pgid == pid) {
        // pgid in background jobs
        if (WIFEXITED(status)) {
          // add completed bg job to completed bg jobs list and remove from jobs list
          int is_positive = jobs[i+1] == NULL ? 1 : 0;

          job_container *job = deep_copy_job(jobs[i]);

          free(job->status);
          job->status = strdup("Done"); 
          job->argc = is_positive;

          // add to completed jobs list
          push_job(&completed_bg_jobs, job->cmd, job->pgid, job->argv, job->argc, job->job_num, job->status);
          // remove from jobs list
          remove_job(jobs[i]);
        }
      }
      i++;
    }
  }
}