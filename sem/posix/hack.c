#include <fcntl.h>
#include <linux/futex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "name.h"

int main(int argc, char*argv[]) {
    const char *sem_file = "/dev/shm/sem.sem";
    int fd = open(sem_file, O_RDWR);
    uint32_t *sem_mem = mmap(NULL, 32, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    printf("Old value: %u\n", *sem_mem);
    *(int*)sem_mem += argc > 1 ? atoi(argv[1]) : 1;
    syscall(SYS_futex, sem_mem, FUTEX_WAKE, 1, NULL, NULL, 0);
    printf("New value: %u\n", *sem_mem);
    munmap(sem_mem, 32);
    close(fd);
}
