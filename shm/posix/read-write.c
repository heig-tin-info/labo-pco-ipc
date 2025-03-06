#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_SIZE 1024

char *shared_memory;

void *writer_thread(void *arg) {
    printf("Writer: writing to shared memory...\n");
    strcpy(shared_memory, "hello!");
    printf("Writer: wrote message: %s\n", shared_memory);
    return NULL;
}

void *reader_thread(void *arg) {
    sleep(1);
    printf("Reader: reading from shared memory...\n");
    printf("Reader: read message: %s\n", shared_memory);
    return NULL;
}

int main() {
    shared_memory = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    pthread_t writer, reader;
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, NULL);

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    munmap(shared_memory, SHM_SIZE);
}
