#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3636

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(sock, (struct sockaddr *)&server_address, sizeof(server_address)) == -1) {
        perror("bind");
        close(sock);
        exit(1);
    }

    char host[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &server_address.sin_addr, host, sizeof(host));
    printf("UDP Server listening on port %s:%d...\n", host, PORT);

    char buffer[1024];
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                                    (struct sockaddr *)&client_address, &client_len);
        if (recv_len > 0) {
            buffer[recv_len] = '\0';
            printf("Received: %s\n", buffer);

            // Répondre au client
            char *response = "Received!";
            sendto(sock, response, strlen(response), 0,
                   (struct sockaddr *)&client_address, client_len);
        }
    }
    close(sock);
}
