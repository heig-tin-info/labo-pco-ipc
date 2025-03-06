#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const char* SHARED_FILE = "/tmp/shared";
const int MAX_BUF = 1024;

void signal_handler(int signum, siginfo_t* siginfo, void* context) {
    if (signum == SIGINT) {
        printf("Cleaning up shared file...\n");
        if (remove(SHARED_FILE) != 0)
            perror("remove");
        exit(0);
    }
}

void lock_file_wait(int fd) {
    struct flock lock = {
        .l_type = F_WRLCK, // Write lock
        .l_whence = SEEK_SET, // Start from beginning of file
        .l_start = 0, .l_len = 0,
        .l_pid = getpid()}; // Current process ID
    // F_SETLKW: Lock the file and wait if it is already locked
    while (fcntl(fd, F_SETLKW, &lock) == -1) {
        if (errno == EINTR) continue;
        perror("fcntl F_SETLKW");
        exit(1);
    }
}

void unlock_file(int fd) {
    struct flock lock = {
        .l_type = F_UNLCK, // Unlock
        .l_whence = SEEK_SET, .l_start = 0, .l_len = 0,
        .l_pid = getpid()};

    // F_SETLK: Set a non-blocking lock
    if (fcntl(fd, F_SETLK, &lock) == -1)
        perror("fcntl");
}

int main(int argc, char** argv) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        // Child process (producer)
        int fd = open(SHARED_FILE, O_RDWR | O_CREAT | O_SYNC, 0666);
        if (fd < 0) {
            perror("CHILD open");
            exit(1);
        }

        int next_number = 0;
        for (;;) {
            lock_file_wait(fd);
            lseek(fd, 0, SEEK_END);
            char buf[MAX_BUF];
            sprintf(buf, "%d\n", next_number);
            write(fd, buf, strlen(buf));
            printf("Write %d\n", next_number);
            next_number++;
            unlock_file(fd);

            sleep(1);
        }

        close(fd);
        exit(0);
    } else {
        // Parent process (consumer)
        struct sigaction action = {.sa_sigaction = signal_handler,
                                   .sa_flags = SA_SIGINFO};
        sigaction(SIGINT, &action, NULL);

        int fd = open(SHARED_FILE, O_RDWR | O_CREAT, 0666);
        if (fd < 0) {
            perror("PARENT open");
            exit(1);
        }

        while (1) {
            lock_file_wait(fd);
            lseek(fd, 0, SEEK_SET);

            char buf[MAX_BUF];
            int n = read(fd, buf, MAX_BUF);
            if (n > 0 && n < MAX_BUF) {
                buf[n] = '\0';
                printf("Read %s", buf);
            }

            if (ftruncate(fd, 0) != 0)
                perror("ftruncate");

            unlock_file(fd);

            sleep(1);
        }

        close(fd);
        wait(NULL);
    }
}
