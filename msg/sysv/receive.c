#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_KEY 1234

struct message {
    long type;
    char text[100];
};

int main() {
    int msgid = msgget(MSG_KEY, 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    struct message msg;
    if (msgrcv(msgid, &msg, sizeof(msg.text), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }

    printf("Message reçu : %s\n", msg.text);

    // Suppression de la file de messages après utilisation
    msgctl(msgid, IPC_RMID, NULL);
}
