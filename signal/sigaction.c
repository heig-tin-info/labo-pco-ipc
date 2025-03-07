#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

FILE* file = NULL;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        printf("\nCleaning up... (this might take some time)\n");
        if (file) {
            fclose(file);
            remove("/tmp/tmpfile");
        }
        sleep(1); // Simulate long cleanup
        exit(0);
    }
}

int main() {
    sigaction(SIGINT, &(struct sigaction){
        .sa_handler=signal_handler}, NULL);

    file = fopen("/tmp/tmpfile", "w");
    while(1) {
        fprintf(file, "Working hard!\n");
        fflush(file);
        sleep(1);
    }
}
