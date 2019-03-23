#include <dispatch/dispatch.h>
#include "csapp.h"
#include "sbuf.h"

#define MAXTHREAD 10

dispatch_semaphore_t mutex;
sbuf_t sbuf;
size_t bytes_cnt;

void echo_init() {
    mutex = dispatch_semaphore_create(1);
}

void echo(int connfd) {
    ssize_t rc;
    char buf[MAXLINE];
    
    static pthread_once_t once = PTHREAD_ONCE_INIT;

    pthread_once(&once, echo_init);

    rio_t rio;
    Rio_readinitb(&rio, connfd);

    while ((rc=Rio_readlineb(&rio, buf, MAXLINE)) > 0) {
        Rio_writen(connfd, buf, rc);
        dispatch_semaphore_wait(mutex, DISPATCH_TIME_FOREVER);
        bytes_cnt += rc;
        printf("Read %ld bytes.\n", bytes_cnt);
        dispatch_semaphore_signal(mutex);
    }
}

void *thread(void *arg) {
    pthread_detach(pthread_self());
    while (1) {
        int connfd = sbuf_remove(&sbuf);
        echo(connfd);
        Close(connfd);
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: echoserver <port>\n");
        exit(0);
    }

    sbuf_init(&sbuf, 10);

    pthread_t tid;
    for (int i=0;i<MAXTHREAD;i++) 
        Pthread_create(&tid, NULL, thread, NULL);

    int listenfd, connfd;
    struct sockaddr_storage client_addr;
    socklen_t client_len;

    listenfd = Open_listenfd(argv[1]);
    while (1) {
        client_len = sizeof (client_addr);
        connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
        sbuf_insert(&sbuf, connfd);
    }

    return 0;
}
