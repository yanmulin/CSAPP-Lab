#include "csapp.h"

void echo(int connfd) {
    size_t n;
    rio_t rio;
    char buf[MAXLINE];

    Rio_readinitb(&rio, connfd);
    while ((n=Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        printf("server received %d bytes\n", (int)n);
        Rio_writen(rio.rio_fd, buf, n);
    }
}

 