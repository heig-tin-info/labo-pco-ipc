#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>

#define PORT 3737
#define MAX_CLIENTS 3

const char *colors[MAX_CLIENTS] = {
    "\033[1;31m", // Red
    "\033[1;32m", // Green
    "\033[1;34m"  // Blue
};

void handle_client(int client_socket, int client_id) {
    char buffer[1024];
    ssize_t bytes_read;

    printf("%sClient %d connected\033[0m\n", colors[client_id], client_id + 1);

    while ((bytes_read = read(client_socket, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%sClient %d: %s\033[0m", colors[client_id], client_id + 1, buffer);
        char *response = "Ack!";
        send(client_socket, response, strlen(response), 0);
    }

    printf("%sClient %d disconnected\033[0m\n", colors[client_id], client_id + 1);
    close(client_socket);
    exit(0);
}

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

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Server listening on port %d...\n", PORT);

    int client_count = 0;

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0) {
            perror("accept");
            continue;
        }

        if (client_count >= MAX_CLIENTS) {
            printf("Max clients reached. Rejecting connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            close(client_socket);
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_fd);
            handle_client(client_socket, client_count);
        } else if (pid > 0) {
            close(client_socket);
            client_count = (client_count + 1) % MAX_CLIENTS;
        } else {
            perror("fork");
            close(client_socket);
        }
    }

    close(server_fd);
}
