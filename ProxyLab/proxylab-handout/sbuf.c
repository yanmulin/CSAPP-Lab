#include "sbuf.h"
#include "csapp.h"

#ifdef __APPLE__
#define V(s) dispatch_semaphore_wait((s), DISPATCH_TIME_FOREVER);
#define P(s) dispatch_semaphore_signal((s));
#else 
#define V(s) sem_wait(&(s))
#define P(s) sem_post(&(s))
#endif

void sbuf_init(sbuf_t *s, int n) {
    s->n = n;
    s->buf = Calloc(n, sizeof (int));
    s->front = s->rear = 0;

#ifdef __APPLE__
    s->mutex = dispatch_semaphore_create(1);
    s->items = dispatch_semaphore_create(0);
    s->slots = dispatch_semaphore_create(n);
#else 
    sem_init(&s->mutex, 0, 1);
    sem_init(&s->items, 0, 0);
    sem_init(&s->slots, 0, n);
#endif

}

void sbuf_insert(sbuf_t *s, int item) {
    V(s->slots);
    V(s->mutex);
    s->buf[(s->front++) % s->n] = item;
    P(s->mutex);
    P(s->items);
}

int sbuf_remove(sbuf_t  *s) {
    V(s->items);
    V(s->mutex);
    int item = s->buf[(s->rear++) % s->n];
    P(s->mutex);
    P(s->slots);
    return item;
}