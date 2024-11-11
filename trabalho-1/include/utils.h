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

void print_statistics_download(time start, time end, size_t total_bytes);
void print_statistics_download_udp(time start, time end, size_t total_bytes, unsigned int total_packages, unsigned int received_packages);

#endif