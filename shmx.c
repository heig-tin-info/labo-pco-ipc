#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_SIZE 1024

char *shared_memory;  // Pointeur vers la mémoire partagée

// Fonction exécutée par le thread écrivain
void *writer_thread(void *arg) {
    printf("Thread Écrivain : Écriture dans la mémoire partagée...\n");
    strcpy(shared_memory, "Hello from thread writer!");
    printf("Thread Écrivain : Message écrit.\n");
    return NULL;
}

// Fonction exécutée par le thread lecteur
void *reader_thread(void *arg) {
    sleep(1);  // Attendre que l'écrivain écrive
    printf("Thread Lecteur : Lecture depuis la mémoire partagée...\n");
    printf("Thread Lecteur : Message lu : %s\n", shared_memory);
    return NULL;
}

int main() {
    pthread_t writer, reader;

    // Création de la mémoire partagée avec mmap()
    shared_memory = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared_memory == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Création des threads
    pthread_create(&writer, NULL, writer_thread, NULL);
    pthread_create(&reader, NULL, reader_thread, NULL);

    // Attente des threads
    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    // Libération de la mémoire partagée
    munmap(shared_memory, SHM_SIZE);

    return 0;
}
