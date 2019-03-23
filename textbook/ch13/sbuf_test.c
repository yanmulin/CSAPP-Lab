#include "sbuf.h"
#include "csapp.h"

#define MAXTHREAD 10

sbuf_t sbuf;

void *thread(void *arg) {
    char buf[MAXLINE];
    long i = (long)arg;
    Pthread_detach(pthread_self());
    printf("Consumer #%ld init.\n", i);
    while (1) {
        int n = sbuf_remove(&sbuf);
        printf("Consumer #%ld: %d\n", i, n);
    }
}

int main() {
    

    sbuf_init(&sbuf, 5);

    pthread_t tid;
    int i = 0;
    for (i=0;i<MAXTHREAD;i++) {
        Pthread_create(&tid, NULL, thread, (void *)(long)i);
    }
    Pthread_create(&tid, NULL, thread, (void*)(long)i);

    printf("Producer init.\n");

    int n = 0;
    while (1) {
        printf("Producer: %d\n", n);
        sbuf_insert(&sbuf, n++);
        sleep(2);
    }

    return 0;
}
