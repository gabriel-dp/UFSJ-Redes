#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

void success(const char *message) {
    printf("[+] %s\n", message);
}

void error(const char *message) {
    printf("[-] %s\n", message);
    exit(1);
}

void warn(const char *message) {
    printf("[!] %s\n", message);
}

time get_time() {
    struct timeval now;
    gettimeofday(&now, NULL);
    return format_time(now.tv_sec, now.tv_usec);
}

time format_time(long int sec, long int usec) {
    time total = sec + ((time)usec / 1000000.0L);
    return total;
}

void print_elapsed_time(char *message, time start, time end) {
    time elapsed = end - start;
    printf("%s = %Lf\n", message, elapsed);
}

FILE *get_file(const char *directory, const char *path, const char *mode) {
    /* Create file path with the specified path */
    file_path full_path = "";
    strcat(full_path, directory);
    strcat(full_path, "/");
    strcat(full_path, path);

    /* Try to open file */
    FILE *file = fopen(full_path, mode);
    if (file == NULL) {
        error("File not found");
    }

    return file;
}