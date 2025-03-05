#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void cleanup(int sig) {
    printf("Received %d, cleaning and exiting...\n", sig);
    exit(0);
}

int main() {
    signal(SIGTERM, cleanup);
    raise(SIGTERM);
}
