#ifndef __SBUF_H
#define __SBUF_H

#ifdef __APPLE__
#include <dispatch/dispatch.h>
#define SEM_TYPE dispatch_semaphore_t
#else 
#include <semaphore.h>
#define SEM_TYPE sem_t
#endif

typedef struct {
    int *buf;
    int n;
    int front;
    int rear;
    SEM_TYPE mutex;
    SEM_TYPE items;
    SEM_TYPE slots;
} sbuf_t;

void sbuf_init(sbuf_t *sbuf, int n);
void sbuf_insert(sbuf_t *sbuf, int item);
int sbuf_remove(sbuf_t *sbuf);

#undef SEM_TYPE
#endif