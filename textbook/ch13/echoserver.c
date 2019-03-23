#include "csapp.h"
#define MAXSET 128

typedef struct {
    int maxfd;
    int maxi;
    fd_set read_set;
    fd_set ready_set;
    int clientfd[MAXSET];
    rio_t clientrio[MAXSET];
} pool_t;

size_t bytes_count = 0;

void init_pool(int listenfd, pool_t *pool) {
    pool->maxi = -1;
    pool->maxfd = listenfd;

    for (int i=0;i<MAXSET;i++) {
        pool->clientfd[i] = -1;
    }

    FD_ZERO(&pool->read_set);
    FD_SET(listenfd, &pool->read_set);
    FD_ZERO(&pool->ready_set);
}

void add_client(pool_t *pool, int clientfd) {
    for (int i=0;i<MAXSET;i++) {
        if (pool->clientfd[i] < 0) {
            pool->clientfd[i] = clientfd;
            FD_SET(clientfd, &pool->read_set);
            Rio_readinitb(&pool->clientrio[i], clientfd);
            if (clientfd > pool->maxfd) 
                pool->maxfd = clientfd;
            if (i > pool->maxi) 
                pool->maxi = i;

            break;
        }
    }
}

void check_pool(pool_t *pool, int nready) {
    char buf[MAXLINE];
    for (int i=0;i<=pool->maxi && nready > 0;i++) {
        if (pool->clientfd[i] > 0 && FD_ISSET(pool->clientfd[i], &pool->ready_set)) {
            nready --;
            int rc = Rio_readlineb(&pool->clientrio[i], buf, MAXLINE);
            if (rc > 0) {
                bytes_count += rc;
                Rio_writen(pool->clientfd[i], buf, rc);
            } else if (rc == 0) {
                Close(pool->clientfd[i]);
                FD_CLR(pool->clientfd[i], &pool->read_set);
                pool->clientfd[i] = -1;
            }
        }
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: echoserver <port>\n");
        exit(0);
    }

    struct sockaddr_storage client_addr;
    socklen_t client_len;
    int listenfd, connfd;
    pool_t pool;

    listenfd = Open_listenfd(argv[1]);
    init_pool(listenfd, &pool);

    while (1) {
        pool.ready_set = pool.read_set;
        int nready = select(pool.maxfd + 1, &pool.ready_set, NULL, NULL, NULL);
        
        if (FD_ISSET(listenfd, &pool.ready_set)) {
            connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
            add_client(&pool, connfd);
            nready--;
        }

        check_pool(&pool, nready);

        printf("Read %lu bytes.\n", bytes_count);
    }

    return 0;
}