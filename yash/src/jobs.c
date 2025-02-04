#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include "../include/common.h"
#include "../include/parser.h"

job_container **jobs = NULL; // Job container type has elements char **argv, int argc, int job_num, and char *status

void free_job_container(job_container *job) {
  if (job == NULL) return;

  if (job->argv != NULL) {
    free_array_mem(job->argv, job->argc);
  }
  
  free(job->status);
  free(job->cmd);
}

void free_jobs_array() {
  if (jobs != NULL) {
    int i = 0;
    while (jobs[i] != NULL) {
      free_job_container(jobs[i]);
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
// #include <stdbool.h>
job_container* peek_job() {
  //read top of jobs stack/list
  if (jobs == NULL || jobs[0] == NULL) return NULL;

  int i = 0;
  while (jobs[i] != NULL) {
    if (jobs[i+1] == NULL) return jobs[i];
    i++;
  }
}

void push_job(char *cmd, pid_t pgid, char *argv[], int argc, char *status) {
  job_container *job = malloc(sizeof(job_container));
  job->cmd = strdup(cmd);
  job->pgid = pgid;
  job->argv = deep_copy_array(argv, argc);
  job->argc = argc;
  job->status = strdup(status);
  job->job_num = peek_job()!=NULL ? peek_job()->job_num + 1 : 1;

  jobs = realloc(jobs, (job->job_num+1)  * sizeof(job_container *));
  jobs[job->job_num] = NULL;
  
  jobs[job->job_num-1] = job;
}

job_container* pop_job() {
  if (jobs == NULL || jobs[0] == NULL) return NULL;

  job_container *job;

  int i = 0;
  while (jobs[i] != NULL) {
    if (jobs[i+1] == NULL) {
      job = jobs[i];
      jobs[i] = NULL;
    }
    i++;
  }
  jobs = realloc(jobs, (i-1) * sizeof(job_container *));
  return job;
}

// char* combine_strings(char *strings[], int size) {
//   // combine array of strings into single string
//   int str_length = 0;
//   for (int i = 0; i < size; i++) {
//     str_length += strlen(strings[i]);
//   }

//   char *string = (char *)malloc(str_length + 1);
//   strcpy(string, "");
//   for (int i = 0; i < size; i++) {
//     // concatenate each string to the single string
//     strcat(string, strings[i]);
//   }

//   return string;
// }

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

// void bg() {

// }

// void fg() {

// }

bool is_jobs_cmd(char *argv[]) {
  //check if command is a valid jobs cmd (i.e., if argv[0] is fg, bg, or jobs)
  char *cmd = argv[0];
  
  if (argv[1] != NULL) return false;
  
  if (strcmp(cmd, "jobs") == 0) {
    jobs_cmd();
  } else if (strcmp(cmd, "fg") == 0) {
    printf("fg!\n");
  } else if (strcmp(cmd, "bg") == 0) {
    printf("bg!\n");
  } else {
    return false;
  }
  return true;
}

void check_bg_processes() {
  
}