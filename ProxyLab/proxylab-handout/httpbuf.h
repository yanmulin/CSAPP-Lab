#ifndef __HTTPBUF_H
#define __HTTPBUF_H

#include <stddef.h>

#define MAXHEADERS 128

#include "csapp.h"

typedef struct {
    char *headers[128];
    char *data;
    size_t data_len;
} httpbuf_t;

httpbuf_t *httpbuf_new();
int httpbuf_readheaders(httpbuf_t *httpbuf, rio_t *rp);
int httpbuf_readdata(httpbuf_t *httpbuf, rio_t *rp);
int httpbuf_read(httpbuf_t *httpbuf, rio_t *rp);
int httpbuf_writeheaders(httpbuf_t *httpbuf, int fd);
int httpbuf_writedata(httpbuf_t *httpbuf, int fd);
int httpbuf_write(httpbuf_t *httpbuf, int fd);
void httpbuf_free(httpbuf_t *httpbuf);

int httpbuf_addheader(httpbuf_t *httpbuf, const char *header);
int httpbuf_modheader(httpbuf_t *httpbuf, const char *field, const char *value);




#endif