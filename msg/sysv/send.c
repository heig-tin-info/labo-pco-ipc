#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "key.h"

struct message {
    long type;
    char text[100];
};

int main(int argc, char *argv[]) {
    int msgid = msgget(MSG_KEY, IPC_CREAT | 0666);
    if (msgid == -1) {
        perror("msgget");
        exit(1);
    }

    char *message = argc > 1 ? argv[1] : "Hello";
    struct message msg = {1};
    strncpy(msg.text, message, sizeof(msg.text));

    if (msgsnd(msgid, &msg, sizeof(msg.text), 0) == -1) {
        perror("msgsnd");
        exit(1);
    }
    printf("Sent: %s\n", msg.text);
}
