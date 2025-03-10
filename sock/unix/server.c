#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mysocket.sock"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[128];

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    unlink(SOCKET_PATH);

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Waiting for connection on %s...\n", SOCKET_PATH);

    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        exit(1);
    }

    read(client_fd, buffer, sizeof(buffer));
    printf("Message from client: %s\n", buffer);

    char *response = "Message received!";
    write(client_fd, response, strlen(response));

    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);
}
