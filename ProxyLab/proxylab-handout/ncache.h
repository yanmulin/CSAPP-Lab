#ifndef __CACHE_H
#define __CACHE_H 

#include <stdlib.h>
#include <semaphore.h>
#include "timestamp.h"
#include "httpbuf.h"

typedef unsigned long long timestamp_t;

typedef struct nc_node {
    char *url;
    httpbuf_t *data;
    size_t size;
    timestamp_t timestamp;
} nc_node_t;

typedef struct {
    nc_node_t **root;
    int n;
    size_t size;
    timestamp_t timestamp;
    int readcnt;
    sem_t mutex;
    sem_t lock;
} ncache_t;

void ncache_init(ncache_t *cache);
nc_node_t *nc_node_new(char *url, timestamp_t timestamp);
void nc_node_free(nc_node_t *node);
void ncache_clear(ncache_t *cache, nc_node_t *node);
nc_node_t *ncache_search(ncache_t *cache, char *url);
void ncache_timestamp_op(ncache_t *cache);

#endif