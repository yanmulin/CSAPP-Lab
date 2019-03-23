#include "csapp.h"

void handler1(int signum) {
    sigset_t mask, prev_mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &prev_mask);
    Sleep(3);
    sio_puts("Sleep over.\n");
    sigprocmask(SIG_SETMASK, &prev_mask, NULL);
}
void handler2(int signum) {
    sio_puts("SIGUSR2");
}
int main() {
    char buf[64];
    if (signal(SIGUSR1, handler1) == SIG_ERR)
        unix_error("signal error");
    if (signal(SIGUSR2, handler2) == SIG_ERR)
        unix_error("signal error");
    pid_t pid = Fork();
    if (pid == 0) {
        while (1);
    }
    kill(pid, SIGUSR1);
    Sleep(1);
    kill(pid, SIGUSR2);
    if (read(STDIN_FILENO, buf, 64) < 0) 
        unix_error("read");
    kill(pid, SIGINT);
    exit(0);
}