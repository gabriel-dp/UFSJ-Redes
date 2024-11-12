#include "../include/utils.h"

#include <getopt.h>
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

void get_args(int argc, char **argv, char **ip, int *port, char **file) {
    /* Initialize default values */
    *port = -1;
    *ip = NULL;
    if (file != NULL) {
        *file = NULL;
    }

    /* Reads data coming from command line */
    int option;
    while ((option = getopt(argc, argv, "i:p:f:")) != -1) {
        switch (option) {
            case 'i':
                *ip = optarg;
                break;
            case 'p':
                *port = atoi(optarg);
                break;
            case 'f':
                *file = optarg;
                break;
            default:
                error("Invalid Args: \n-i <ip>   [required] \n-p <port> [required] \n-f <file> [optional] [client-only]");
        }
    }

    /* Checks required flags */
    if (*port == -1) error("Port number is missing on args");
    if (*ip == NULL) error("IP is missing on args");
}

time get_time() {
    struct timeval now;
    gettimeofday(&now, NULL);
    time total = now.tv_sec + ((time)now.tv_usec / 1000000.0L);
    return total;
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
    printf("Elapsed time (seconds) = %Lf\n", end - start);
    printf("Size (bytes) = %ld\n", total_bytes);
    printf("Speed (bytes/second) = %Lf\n", total_bytes / (end - start));
    printf("\n");
}

void print_statistics_packages(unsigned long int total_packages, unsigned long int received_packages) {
    long double package_loss = (double)(total_packages - received_packages) / total_packages;
    printf("Package Loss = %.2Lf%% (%ld/%ld)\n", package_loss * 100, received_packages, total_packages);
}
