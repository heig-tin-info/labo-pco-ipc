#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 3737

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = INADDR_ANY,
        .sin_port = htons(PORT)
    };

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) { // Boucle infinie pour accepter plusieurs clients
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("accept");
            continue; // Continue d'accepter les nouvelles connexions
        }

        printf("Client from %s:%d connected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        char buffer[1024] = {0};
        ssize_t bytes_read;
        while ((bytes_read = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0'; // S'assurer que le message est bien terminé
            printf("Received message: %s\n", buffer);

            char *response = "Ack!";
            send(client_socket, response, strlen(response), 0);
        }

        printf("Client from %s:%d disconnected\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        close(client_socket);
    }

    close(server_fd);
}
