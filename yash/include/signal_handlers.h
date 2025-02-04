#ifndef SIGNAL_HANDLERS_H
#define SIGNAL_HANDLERS_H

void handle_SIGINT(int signal);

void handle_SIGTSTP(int signal);

void handle_SIGQUIT(int signal);

void handle_SIGCHLD(int signal);

#endif