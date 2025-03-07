#include <stdio.h>
#include <unistd.h>

int main() {
    // Set an alarm to go off in N seconds
    alarm(2);

    // Wake up the process once a signal is received
    pause();
}
