#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "key.h"

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

    printf("Received: %s\n", msg.text);
}
