#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/posix_queue"

int main() {
    mqd_t mq = mq_open(QUEUE_NAME, O_RDONLY);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    char buffer[256];
    if (mq_receive(mq, buffer, 256, NULL) == -1) {
        perror("mq_receive");
        exit(1);
    }

    printf("Message reçu : %s\n", buffer);
    mq_close(mq);
    mq_unlink(QUEUE_NAME);  // Supprimer la file après utilisation
}
