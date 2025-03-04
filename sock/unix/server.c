#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/mysocket"

int main() {
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[128];

    // 1. Créer le socket UNIX
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // 2. Configurer l'adresse du socket
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    // 3. Supprimer l'ancien socket s'il existe
    unlink(SOCKET_PATH);

    // 4. Associer le socket à l'adresse (bind)
    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(1);
    }

    // 5. Écouter les connexions entrantes
    if (listen(server_fd, 5) == -1) {
        perror("listen");
        exit(1);
    }

    printf("Serveur en attente de connexion sur %s...\n", SOCKET_PATH);

    // 6. Accepter une connexion
    client_fd = accept(server_fd, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        exit(1);
    }

    // 7. Lire un message du client
    read(client_fd, buffer, sizeof(buffer));
    printf("Message reçu du client : %s\n", buffer);

    // 8. Répondre au client
    char *response = "Message reçu par le serveur.";
    write(client_fd, response, strlen(response));

    // 9. Fermer la connexion
    close(client_fd);
    close(server_fd);
    unlink(SOCKET_PATH);  // Supprimer le fichier socket

    return 0;
}
