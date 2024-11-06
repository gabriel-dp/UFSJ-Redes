#include "../include/utils.h"

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