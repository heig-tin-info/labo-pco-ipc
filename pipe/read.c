#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char** argv) {
    // Qu'il s'agisse d'un fichier régulier ou d'un tube nommé,
    // le principe de base est le même : on ouvre un descripteur de fichier
    // en lecture et on lit les données qui y sont écrites.
    int fd = open("/tmp/fifo", O_RDONLY);

    char buffer[1024];
    ssize_t read_bytes;
    while ((read_bytes = read(fd, buffer, sizeof(buffer))) > 0) {
        buffer[read_bytes] = '\0';
        printf("Read %ld bytes: %s\n", read_bytes, buffer);
    }
    if (read_bytes == 0)
        printf("EOF\n");
    else if (read_bytes == -1)
        perror("read");

    close(fd);
}
