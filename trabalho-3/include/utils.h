#ifndef UTILS_H
#define UTILS_H

#define BUFFER_SIZE 1024
#define BUFFER_HEADER_SIZE 6
#define BUFFER_DATA_SIZE (BUFFER_SIZE - BUFFER_HEADER_SIZE)

void success(const char *message);
void error(const char *message);
void warn(const char *message);

void get_args(int argc, char **argv, char **ip, int *port, char **file);

int encode(char *buffer, long package_id, long total_packages, char *data);
int decode(char *buffer, long *package_id, long *total_packages, char *data);

int random_error();

#endif