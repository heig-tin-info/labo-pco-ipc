#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/posix_queue"

int main() {
    mqd_t mq;
    struct mq_attr attr = { .mq_flags = 0, .mq_maxmsg = 10, .mq_msgsize = 256, .mq_curmsgs = 0 };

    mq = mq_open(QUEUE_NAME, O_CREAT | O_WRONLY, 0666, &attr);
    if (mq == -1) {
        perror("mq_open");
        exit(1);
    }

    char message[] = "Hello, POSIX Queue!";
    if (mq_send(mq, message, sizeof(message), 0) == -1) {
        perror("mq_send");
        exit(1);
    }

    printf("Message envoyé : %s\n", message);
    mq_close(mq);
}
