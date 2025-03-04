#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mysocket"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[128];

    // 1. Créer le socket UNIX
    client_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("socket");
        exit(1);
    }

    // 2. Configurer l'adresse du serveur
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Se connecter au serveur
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(1);
    }

    // 4. Envoyer un message au serveur
    char *message = "Hello, UNIX Socket!";
    write(client_fd, message, strlen(message));

    // 5. Lire la réponse du serveur
    read(client_fd, buffer, sizeof(buffer));
    printf("Réponse du serveur : %s\n", buffer);

    // 6. Fermer la connexion
    close(client_fd);
}
