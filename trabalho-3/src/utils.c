#include "utils.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    *file = NULL;

    /* Read data coming from command line */
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
                error("Invalid Args: \n-i <ip>   [required] \n-p <port> [required] \n-f <file> [required]");
        }
    }

    /* Check required flags */
    if (*ip == NULL) error("IP is missing on args");
    if (*port == -1) error("Port number is missing on args");
    if (*file == NULL) error("File is missing on args");
}

int encode(char *buffer, long package_id, long total_packages, char *data) {
    /* Encode package id to 3 bytes */
    buffer[0] = (package_id >> 16) & 0xFF;
    buffer[1] = (package_id >> 8) & 0xFF;
    buffer[2] = package_id & 0xFF;

    /* Encode total packages to 3 bytes */
    buffer[3] = (total_packages >> 16) & 0xFF;
    buffer[4] = (total_packages >> 8) & 0xFF;
    buffer[5] = total_packages & 0xFF;

    /* Copy data to buffer */
    memcpy(buffer + BUFFER_HEADER_SIZE, data, BUFFER_DATA_SIZE);

    return 1;
}

int decode(char *buffer, long *package_id, long *total_packages, char *data) {
    /* Decode package id from 3 bytes */
    *package_id = ((unsigned char)buffer[0] << 16) | ((unsigned char)buffer[1] << 8) | (unsigned char)buffer[2];

    /* Decode total packages from 3 bytes */
    *total_packages = ((unsigned char)buffer[3] << 16) | ((unsigned char)buffer[4] << 8) | (unsigned char)buffer[5];

    /* Copy data from buffer */
    memcpy(data, buffer + BUFFER_HEADER_SIZE, BUFFER_DATA_SIZE);

    return 1;
}