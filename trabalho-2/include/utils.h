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

void get_args(int argc, char **argv, char **ip, int *port, char **file);

#endif