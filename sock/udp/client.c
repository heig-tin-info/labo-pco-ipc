#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3636
#define HOST "127.0.0.1"

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0); // UDP
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET, // IPv4
        .sin_port = htons(PORT)
    };
    inet_pton(AF_INET, HOST, &server_address.sin_addr);

    char *message = "Hello Server!";
    sendto(sock, message, strlen(message), 0,
        (struct sockaddr *)&server_address, sizeof(server_address));

    char buffer[1024] = {0};
    struct sockaddr_in from_address;
    socklen_t from_len = sizeof(from_address);
    recvfrom(sock, buffer, sizeof(buffer), 0,
        (struct sockaddr *)&from_address, &from_len);

    printf("Answer: %s\n", buffer);

    close(sock);
}
