#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

#define SERVER_DIRECTORY "tests/server"
#define CLIENT_DIRECTORY "tests/client"
#define BUFFER_SIZE 1024

typedef char file_path[512];
typedef long double time;

void success(const char *message);
void error(const char *message);
void warn(const char *message);

time get_time();
time format_time(long int sec, long int usec);
void print_elapsed_time(char *message, time start, time end);

FILE *get_file(const char *directory, const char *path, const char *mode);

#endif