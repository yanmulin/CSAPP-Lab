#include "timestamp.h"

#define V(s) sem_wait((s))
#define P(s) sem_post((s))

void timestamp_init(tsbuf_t *ts) {
    sem_init(&ts->mutex, 0, 1);
    sem_init(&ts->lock, 0, 1);
    ts->timestamp = 0;
    ts->readcnt = 0;
}

timestamp_t timestamp_now(tsbuf_t *ts) {
    V(&ts->mutex);
    ts->readcnt ++;
    if (ts->readcnt == 1)
        V(&ts->lock);
    P(&ts->mutex);

    timestamp_t ret = ts->timestamp;

    V(&ts->mutex);
    ts->readcnt ++;
    if (ts->readcnt == 1)
        P(&ts->lock);
    P(&ts->mutex);

    return ret;
}

void timestamp_op(tsbuf_t *ts) {
    V(&ts->lock);
    ts->timestamp ++;
    P(&ts->lock);
}