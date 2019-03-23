#include "csapp.h"

void handler1(int sig) {
    int olderrno = errno;
    pid_t pid;
    while ((pid=waitpid(-1, NULL, 0)) > 0)
        sio_puts("Handler reaped child\n");
    Sleep(1);
    errno = olderrno;
}

int main() {
    int i;
    char buf[64];
    if (signal(SIGCHLD, handler1) == SIG_ERR) 
        unix_error("signal error");
    
    for (i=0;i<3;i++) {
        if (Fork() == 0) {
            printf("Hello from child %d\n", (int)getpid());
            exit(0);
        }
    }

    if (read(STDIN_FILENO, buf, 64) < 0)
        unix_error("read");
    
    printf("Parent processing input\n");
    while (1);
    exit(0);
}