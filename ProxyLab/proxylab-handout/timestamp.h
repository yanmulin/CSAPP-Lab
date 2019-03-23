#ifndef __TIMESTAMP_H
#define __TIMESTAMP_H

#include <semaphore.h>

typedef unsigned long long timestamp_t;

typedef struct {
    timestamp_t timestamp;
    sem_t mutex;
    sem_t lock;
    int readcnt;
} tsbuf_t;

void timestamp_init(tsbuf_t *ts);
timestamp_t timestamp_now(tsbuf_t *ts);
void timestamp_op(tsbuf_t *ts);

#endif