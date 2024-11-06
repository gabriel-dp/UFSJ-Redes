#include "../include/utils.h"

#include <stdio.h>
#include <stdlib.h>

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