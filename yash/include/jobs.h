#ifndef JOBS_H
#define JOBS_H

#include <stdbool.h>
#include <common.h>
#include <sys/types.h>

extern job_container **jobs;
extern job_container **completed_bg_jobs;

void free_job_container(job_container *job);

void free_jobs_array(job_container ***jobs_array);

char **deep_copy_array(char *array[], int size);

job_container* deep_copy_job(job_container *job);

void remove_job(job_container *job);

job_container* peek_job();

void push_job(job_container ***jobs_list, char *cmd, pid_t pgid, char *argv[], int argc, int job_num, char *status);

job_container* pop_job(job_container ***jobs_list, bool is_FIFO);

//char* combine_strings(char *strings[], int size);

// void bg();

// void fg();

void jobs_cmd();

bool is_jobs_cmd(char *argv[]);

void check_bg_jobs();

#endif