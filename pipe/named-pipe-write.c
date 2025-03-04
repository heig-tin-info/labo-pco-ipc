#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char** argv) {
    const char* FIFO_PATH = "/tmp/fifo";
    int fifo_status = mkfifo(FIFO_PATH, 0666);
    if (fifo_status == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

    // Open is blocking until the other end of the pipe is opened
    int fd = open(FIFO_PATH, O_WRONLY);
    char buffer[1024];
    do {
        printf("> ");
        fgets(buffer, sizeof(buffer), stdin);
    } while(write(fd, buffer, strcspn(buffer, "\n")) > 0);

    close(fd);
}
