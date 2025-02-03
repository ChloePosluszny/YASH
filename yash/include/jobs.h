#ifndef JOBS_H
#define JOBS_H

#include <stdbool.h>
#include <common.h>

extern job_container *jobs[];

void free_job_container(job_container *job);

void free_jobs_array();

char **deep_copy_array(char *array[], int size);

job_container* peek_job();

void push_job(char *cmd, char *argv[], int argc, char *status);

job_container* pop_job();

char* combine_strings(char *strings[]);

void jobs_cmd();

bool is_jobs_cmd(char *argv[]);

#endif