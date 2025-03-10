#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mysocket.sock"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[128];

    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    char *message = "Hello, UNIX Socket!";
    write(client_fd, message, strlen(message));

    read(client_fd, buffer, sizeof(buffer));
    printf("Got an answer: %s\n", buffer);

    close(client_fd);
}
