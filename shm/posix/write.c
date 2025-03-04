#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>

int main() {
    // Create a shared memory segment
    int fd = shm_open("/posix_shm", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }

    // By default, provided file has a size of 0 bytes
    // We need to set the size of the shared memory segment
    const size_t size = 1024;
    if (ftruncate(fd, size) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }

    // Map the shared memory segment into the process space
    char *shared_memory = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    // Write something in the shared memory
    strcpy(shared_memory, "Hello from POSIX SHM!");
    printf("Wrote: %s\n", shared_memory);

    // Detach the shared memory segment
    munmap(shared_memory, size);
    close(fd);
}
