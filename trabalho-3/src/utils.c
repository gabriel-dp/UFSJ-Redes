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