#include <stdio.h> 
#include "csapp.h"

void sigint_handler(int signo) {
    sio_puts("foo SIGINT\n");
    exit(0);
}

int main() {
    Signal(SIGINT, sigint_handler);
    int n = 10;
    // printf("foo foreground pgid: %d\n", tcgetpgrp(STDIN_FILENO));
    for (int i=0;i<n;i++) {
        sleep(1);
    }
    printf("foo Done.\n");
    return 0;
}