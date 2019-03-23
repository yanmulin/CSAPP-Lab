#include "ncache.h"
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

#define V(s) sem_wait((s))
#define P(s) sem_post((s))

void ncache_init(ncache_t *cache) {
    cache->root = (nc_node_t **)Calloc(128, sizeof (nc_node_t));
    cache->size = 0;
    cache->timestamp = 0;
    cache->readcnt = 0;
    sem_init(&cache->mutex, 0, 1);
    sem_init(&cache->lock, 0, 1);
}

static void ncache_insert(ncache_t *cache, nc_node_t *node) {
    V(&cache->lock);

    int i, n = cache->n;
    nc_node_t **list = cache->root;

    for (i=0;i<n;i++) {
        if (list[i]==NULL) break;
    }

    list[i] = node;
    node->timestamp = cache->timestamp;
    cache->size += node->size;
    if (i == n) cache->n++;

    P(&cache->lock);
}

static void ncache_remove(ncache_t *cache) {
    V(&cache->lock);

    int n = cache->n;
    timestamp_t minstamp = cache->timestamp;
    nc_node_t **list = cache->root;
    int midx = -1;

    for (int i=0;i<n;i++) {
        if (list[i] && list[i]->timestamp < minstamp) {
            midx = i;
            minstamp = list[i]->timestamp;
        }    
    }

    if (midx >= 0) {
        cache->size -= list[midx]->size;
        nc_node_free(list[midx]);
        list[midx] = NULL;
    }

    P(&cache->lock);
}

static size_t ncache_size(ncache_t *cache) {
    V(&cache->mutex);
    cache->readcnt ++;
    if (cache->readcnt == 1) 
        V(&cache->lock);
    P(&cache->mutex);

    size_t size = cache->size;

    V(&cache->mutex);
    cache->readcnt --;
    if (cache->readcnt == 0) 
        P(&cache->lock);
    P(&cache->mutex);

    return size;
}

nc_node_t *nc_node_new(char *url, timestamp_t timestamp) {
    nc_node_t *node = (nc_node_t *)Malloc(sizeof (nc_node_t));
    node->url = (char *)Calloc(strlen(url) * 2, sizeof (char));
    strcpy(node->url, url);
    node->timestamp = timestamp;
    return node;
}

void nc_node_free(nc_node_t *node) {
    free(node->url);
    httpbuf_free(node->data);
    free(node);
}

void ncache_clear(ncache_t *cache, nc_node_t *node) {
    if (node->data->data_len > MAX_OBJECT_SIZE) {
        nc_node_free(node);
    } else {
        while (node->data->data_len + ncache_size(cache) > MAX_CACHE_SIZE) {
            ncache_remove(cache);
        }
        ncache_insert(cache, node);
    }
}

nc_node_t *ncache_search(ncache_t *cache, char *url) {
    V(&cache->mutex);
    cache->readcnt ++;
    if (cache->readcnt == 1) 
        V(&cache->lock);
    P(&cache->mutex);

    int n = cache->n;
    nc_node_t **list = cache->root;
    nc_node_t *node = NULL;
    for (int i=0;i<n;i++) {
        if (list[i] != NULL && strcmp(list[i]->url, url) == 0){
            node = list[i];
            break;
        }
    }

    V(&cache->mutex);
    cache->readcnt --;
    if (cache->readcnt == 0) 
        P(&cache->lock);
    P(&cache->mutex);

    return node;
}