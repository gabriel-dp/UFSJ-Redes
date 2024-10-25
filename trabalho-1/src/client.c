#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void communicate(int sock) {
    char buffer[1024];
    bzero(buffer, sizeof(buffer));

    printf("\nSending to server: ");
    scanf("%1023s", buffer);
    write(sock, buffer, sizeof(buffer));

    bzero(buffer, sizeof(buffer));
    read(sock, buffer, sizeof(buffer));
    printf("Received from server: %s\n\n", buffer);
}

int main() {
    /* Basic network connection variables */
    char *ip = "127.0.0.1";
    int port = 8000;

    /* Socket variables*/
    struct sockaddr_in addr;

    /* Create socket */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("[-] Socket error");
        exit(1);
    }
    printf("[+] TCP client socket created\n");

    /* Set address info */
    memset(&addr, '\0', sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    /* Connect to server */
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[-] Connect error");
        close(sock);  // Close the socket before exiting
        exit(1);
    }
    printf("[+] Connected to the server\n");

    /* Send and receive data */
    communicate(sock);

    /* Close the socket */
    close(sock);
    printf("[+] Disconnected\n");
    return 0;
}
