#include "csapp.h"

void command() {
    char buf[MAXLINE];
    if (!Fgets(buf, MAXLINE, stdin)) {
        exit(0);
    }
    printf("%s", buf);
}

void echo(int connfd) {
    char buf[MAXLINE];
    rio_t rio;
    Rio_readinitb(&rio, connfd);
    size_t rc = Rio_readlineb(&rio, buf, MAXLINE);
    if (rc > 0) {
        Rio_writen(connfd, buf, rc);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: echo <port>\n");
        exit(0);
    }

    fd_set read_set, ready_set;
    struct sockaddr_storage client_addr;
    socklen_t client_len;
    int listenfd, connfd;

    listenfd = Open_listenfd(argv[1]);

    FD_SET(listenfd, &read_set);
    FD_SET(STDIN_FILENO, &read_set);


    while (1) {
        ready_set = read_set;
        int n = select(listenfd+1, &ready_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &ready_set)) {
            command();
        }
        if (FD_ISSET(listenfd, &ready_set)) {
            client_len = sizeof (client_addr);
            connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
            echo(connfd);
        }
    }

    return 0;
}


