#include "../include/utils.h"

#include <math.h>
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
        warn("File not found");
    }

    return file;
}

void print_statistics_download(time start, time end, size_t total_bytes) {
    print_elapsed_time("Elapsed time (seconds)", start, end);
    printf("Size (bytes) = %ld\n", total_bytes);
    printf("Speed (bytes/second) = %Lf\n\n", total_bytes / (end - start));
}

void print_statistics_download_udp(time start, time end, size_t total_bytes, unsigned int total_packages, unsigned int received_packages) {
    double package_loss = (double)(total_packages - received_packages) / total_packages;

    printf("Package Loss = %.2lf%% (%d/%d)\n", package_loss * 100, received_packages, total_packages);
    print_statistics_download(start, end, total_bytes);
}
