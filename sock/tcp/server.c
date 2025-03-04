#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

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

    if (listen(server_fd, 5 /* connexions */) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);


    int new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("accept");
        exit(1);
    }

    char buffer[1024] = {0};
    read(new_socket, buffer, 1024);
    printf("Received message: %s\n", buffer);

    char *response = "Ack!";
    send(new_socket, response, strlen(response), 0);

    close(new_socket);
    close(server_fd);
}
