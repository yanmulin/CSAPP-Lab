#include "csapp.h"

extern void echo(int);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: echoserver <port>\n");
        exit(0);
    }    

    struct sockaddr_storage clientaddr;
    socklen_t client_len;
    char *port = argv[1];
    int listenfd = Open_listenfd(port);
    char client_hostname[MAXLINE];
    char client_port[MAXLINE];
    while (1) {
        client_len = sizeof(struct sockaddr_storage); 
        int clientfd = Accept(listenfd, (SA *)&clientaddr, &client_len);
        Getnameinfo((SA*)&clientaddr, client_len, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);
        echo(clientfd);
        Close(clientfd);
    } 
    return 0;
}