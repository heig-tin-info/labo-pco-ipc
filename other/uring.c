#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <liburing.h>

#define FILE_NAME "uring.c"

int main() {
    int fd = open(FILE_NAME, O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    struct io_uring ring;
    io_uring_queue_init(8, &ring, 0);  // Init une queue de 8 opérations

    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    struct io_uring_cqe *cqe;

    char buffer[128];
    io_uring_prep_read(sqe, fd, buffer, sizeof(buffer), 0);
    io_uring_submit(&ring);

    io_uring_wait_cqe(&ring, &cqe);
    printf("Données lues : %s\n", buffer);
    io_uring_cqe_seen(&ring, cqe);

    io_uring_queue_exit(&ring);
    close(fd);
    return 0;
}
