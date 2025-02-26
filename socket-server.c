#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    char buffer[1024] = {0};

    // Création de la socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(1);
    }

    // Configuration de l'adresse
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Associer la socket à l'adresse
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        exit(1);
    }

    // Écouter les connexions
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(1);
    }

    printf("Serveur en attente de connexions sur le port %d...\n", PORT);

    // Accepter un client
    new_socket = accept(server_fd, NULL, NULL);
    if (new_socket < 0) {
        perror("accept");
        exit(1);
    }

    // Lire les données envoyées par le client
    read(new_socket, buffer, 1024);
    printf("Message reçu du client : %s\n", buffer);

    // Répondre au client
    char *response = "Message reçu par le serveur";
    send(new_socket, response, strlen(response), 0);

    // Fermer la socket
    close(new_socket);
    close(server_fd);

    return 0;
}
