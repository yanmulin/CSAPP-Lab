#include "csapp.h"

pid_t pid;

void sigchld_handler(int signo) {
    int olderrno = errno;
    pid = Waitpid(-1, NULL, 0);
    errno = olderrno;
}
void sigint_handler(int signo) {
    sio_puts("receive SIGINT\n");
}

int main() {
    sigset_t mask, prev;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);

    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);

    while (1) {
        sigprocmask(SIG_BLOCK, &mask, &prev);
        if (Fork() == 0) {
            sigprocmask(SIG_UNBLOCK, &prev, NULL);
            exit(0);
        }
        pid = 0;
        while (!pid)
            sigsuspend(&prev);

        sigprocmask(SIG_SETMASK, &prev, NULL);

        printf(".");

    }

    exit(0);
}