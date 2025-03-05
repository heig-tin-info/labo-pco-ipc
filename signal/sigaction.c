#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    struct sigaction sa = {
        // Dans la très grande majorité des cas, on met 0
        .sa_flags = 0,

        // Quels signaux seront ignorés (masqués)
        // Il ne doit pas être modifié manuellement, on utilise
        // sigemptyset() et sigaddset() pour le manipuler
        .sa_mask = 0x00,

        // Obsolète et non POSIX
        .sa_restorer = NULL
    };

    // Efface tous les signaux du masque
    sigemptyset(&sa.sa_mask);

    // Ajoute tous les signaux au masque
    sigfillset(&sa.sa_mask);

    // Supprime un signal particulier du masque
    sigdelset(&sa.sa_mask, SIGINT);

    // Ajoute un signal particulier au masque
    sigaddset(&sa.sa_mask, SIGUSR1);

    // Crée un nouveau masque utilisateur
    sigset_t my_mask, oldmask;
    sigemptyset(&my_mask);
    sigaddset(&my_mask, SIGUSR1);

    // Bloque les signaux à attendre
    sigprocmask(SIG_BLOCK, &my_mask, &oldmask);

    // Met le processus en pause et attend un des
    // signaux du masque, ici SIGUSR1
    sigsuspend(&my_mask);

    // Restaure le masque original
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
