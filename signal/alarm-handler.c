#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void signal_handler(int sig) {
    if (sig == SIGALRM)
        printf("Alarm went off!\n");
}

int main() {
    // Register a signal handler
    signal(SIGALRM, signal_handler);

    // Set an alarm to go off in N seconds
    alarm(2);

    // Wake up the process once a signal is received
    pause();
}
