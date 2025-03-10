#define _POSIX_C_SOURCE 200809L

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

void signal_handler(int signum, siginfo_t *info, void *ctx) {
    if (signum == SIGINT) {
        printf("Cleaning up shared file...\n");
        if (remove(SHARED_FILE) != 0)
            perror("remove");
        exit(0);
    }
}

/**
 * Lock the file and wait until it is unlocked.
 * This function is blocking.
 */
void lock_file_wait(int fd) {
    struct flock lock = {
        .l_type = F_WRLCK, // Write lock
        .l_whence = SEEK_SET, // Start from beginning of file
        .l_start = 0,
        .l_len = 0,
        .l_pid = getpid()
    };
    // F_SETLKW: Lock the file and wait if it is already locked
    while (fcntl(fd, F_SETLKW, &lock) == -1) {
        if (errno == EINTR) continue;
        perror("fcntl F_SETLKW");
        exit(1);
    }
}

/**
 * Unlock the file.
 */
void unlock_file(int fd) {
    struct flock lock = {
        .l_type = F_UNLCK, // Unlock
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
        .l_pid = getpid()
};

    // F_SETLK: Set a non-blocking lock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("fcntl F_SETLK");
        exit(1);
    }
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
            // --- Begin Critical Section ---
            lseek(fd, 0, SEEK_END);
            char buf[MAX_BUF];
            sprintf(buf, "%d\n", next_number);
            write(fd, buf, strlen(buf));
            printf("Write %d\n", next_number);
            sleep(1); // Simulate work
            next_number++;
            // --- End Critical Section ---
            unlock_file(fd);
            sleep(1);
        }

        close(fd);
        exit(0);
    } else {
        // Parent process (consumer)
        struct sigaction action = { .sa_sigaction = signal_handler };
        sigaction(SIGINT, &action, NULL);

        int fd = open(SHARED_FILE, O_RDWR | O_CREAT, 0666);
        if (fd < 0) {
            perror("PARENT open");
            exit(1);
        }

        while (1) {
            lock_file_wait(fd);
            // --- Begin Critical Section ---
            lseek(fd, 0, SEEK_SET);

            char buf[MAX_BUF];
            int n = read(fd, buf, MAX_BUF);
            if (n > 0 && n < MAX_BUF) {
                buf[n] = '\0';
                printf("Read %s", buf);
                sleep(1); // Simulate work
            }
            if (ftruncate(fd, 0) != 0)
                perror("ftruncate");
            // --- End Critical Section ---
            unlock_file(fd);
            sleep(1);
        }
        close(fd);
        wait(NULL);
    }
}
