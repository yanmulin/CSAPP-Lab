#include "csapp.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <domain name>\n", argv[0]);
        exit(0);
    }

    struct addrinfo *listp, hints;
    int rc;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((rc=getaddrinfo(argv[1], NULL, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(rc));
        exit(1);
    }
    int flags = NI_NUMERICHOST;
    struct addrinfo *p;
    char buf[MAXLINE];
    for (p=listp;p!=NULL;p=p->ai_next) {
        // Getnameinfo(p->ai_addr, p->ai_addrlen, buf, MAXLINE, NULL, 0, flags);
        struct sockaddr_in *sin = (struct sockaddr_in*)(p->ai_addr);
        inet_ntop(AF_INET, &(sin->sin_addr), buf, MAXLINE);
        printf("%s\n", buf);
    }
    Freeaddrinfo(listp);
    return 0;
}