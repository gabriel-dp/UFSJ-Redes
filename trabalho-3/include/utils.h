#ifndef UTILS_H
#define UTILS_H

void success(const char *message);
void error(const char *message);
void warn(const char *message);

void get_args(int argc, char **argv, char **ip, int *port, char **file);

#endif