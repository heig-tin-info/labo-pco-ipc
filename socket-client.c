#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    // Création de la socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        exit(1);
    }

    // Configuration du serveur distant
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("connect");
        exit(1);
    }

    // Envoyer un message au serveur
    char *message = "Hello Server!";
    send(sock, message, strlen(message), 0);

    // Lire la réponse du serveur
    read(sock, buffer, 1024);
    printf("Réponse du serveur : %s\n", buffer);

    // Fermer la socket
    close(sock);

    return 0;
}
