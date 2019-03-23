#include "csapp.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: echoclient <host> <post>\n");
        exit(0);
    }
    
    char *host = argv[1];
    char *port = argv[2];
    int clientfd = Open_clientfd(host, port);

    rio_t rio;
    char buf[MAXLINE];
    Rio_readinitb(&rio, clientfd);
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        Rio_readlineb(&rio, buf, MAXLINE);
        Fputs(buf, stdout);
    }
    Close(clientfd);
    return 0;
}