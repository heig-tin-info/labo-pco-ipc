#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 3737
#define BUFFER_SIZE 1024

int sock;

void handle_signal(int sig) {
    printf("\nClosing client...\n");
    close(sock);
    exit(0);
}

int main() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT)
    };
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect");
        exit(1);
    }

    printf("Connected to server on port %d. Type your messages (Ctrl+D to exit).\n", PORT);

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nDisconnecting...\n");
            break;
        }

        send(sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = read(sock, buffer, sizeof(buffer));
        if (bytes_received <= 0) {
            printf("Server closed the connection.\n");
            break;
        }
        printf("\033[1;34m%s\033[0m\n", buffer);
    }

    close(sock);
}
