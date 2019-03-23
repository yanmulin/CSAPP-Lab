#include "httpbuf.h"

static int httpbuf_findheader(httpbuf_t *httpbuf, const char *field);
static void httpbuf_freeheaders(httpbuf_t *httpbuf);

httpbuf_t *httpbuf_new() {
    httpbuf_t *pb = (httpbuf_t *)Malloc(sizeof (httpbuf_t));
    pb->data_len = 0;
    return pb;
}

int httpbuf_readheaders(httpbuf_t *httpbuf, rio_t *rp) {
    char buf[MAXLINE];
	size_t hidx = 0, rc;
	size_t data_len = 0;
    while ((rc=Rio_readlineb(rp, buf, MAXLINE)) > 0) {     
		if (strncasecmp(buf, "Content-Length: ", 16) == 0) {
			data_len = atoi(&buf[16]);
		}
		if (hidx == MAXHEADERS - 1) continue;
		if (strcmp(buf, "\r\n") == 0) break;
		else {
			httpbuf->headers[hidx] = (char*)Malloc((strlen(buf) + 1) * sizeof (char));
			strcpy(httpbuf->headers[hidx++], buf);
		}
    }
	httpbuf->headers[hidx] = NULL;
    httpbuf->data_len = data_len;
    if (rc < 0) return -1;
    else return 0;
}

int httpbuf_readdata(httpbuf_t *httpbuf, rio_t *rp) {
    if (httpbuf->data_len <= 0) return 0;
    size_t data_len = httpbuf->data_len;
    ssize_t rc, rn;
    rc = rn = 0;
    httpbuf->data = (char*)Calloc(data_len, sizeof (char));
    while (rn < data_len && (rc=Rio_read(rp, httpbuf->data+rn, data_len-rn)) > 0) {
        rn += rc;
    }
    if (rc < 0) return -1;
    else return 0;
}

int httpbuf_read(httpbuf_t *httpbuf, rio_t *rp) {
    if (httpbuf_readheaders(httpbuf, rp) < 0) {
        printf("READ HEADERS ERROR.\n");
        return -1;
    }
    if (httpbuf_readdata(httpbuf, rp) < 0) {
        printf("READ DATA ERROR: data_len=%ld\n", httpbuf->data_len);
        return -1;
    }
    return 0;
}

int httpbuf_writeheaders(httpbuf_t *httpbuf, int fd) {
	for (size_t idx=0;httpbuf->headers[idx]!=NULL&&idx<MAXHEADERS;idx++) {
		ssize_t wt = rio_writen(fd, httpbuf->headers[idx], strlen(httpbuf->headers[idx]));
        if (wt <= 0) return -1;
	}

    char buf[MAXLINE];
	snprintf(buf, MAXLINE,  "\r\n");
	if (rio_writen(fd, "\r\n", strlen(buf)) < 0) return -1;
    else return 0;
}

int httpbuf_writedata(httpbuf_t *httpbuf, int fd) {
    if (httpbuf->data_len <= 0) return 0;
    ssize_t wt = 0, wn = 0;
    size_t data_len = httpbuf->data_len;
    while (data_len > wn && (wt=rio_writen(fd, httpbuf->data + wn, data_len - wn)) > 0) {
        wn += wt;
    }
    if (wt < 0) return -1;
    else return 0;
}

int httpbuf_write(httpbuf_t *httpbuf, int fd) {
    if (httpbuf_writeheaders(httpbuf, fd) < 0) return -1;
    if (httpbuf_writedata(httpbuf, fd) < 0) return -1;
    return 0;
}


int httpbuf_addheader(httpbuf_t *httpbuf, const char *header) {
	int i;
	for (i=0;httpbuf->headers[i]!=NULL&&i<MAXHEADERS;i++);
	if (i == MAXHEADERS) return -1;
	httpbuf->headers[i] = (char *)Calloc(strlen(header), sizeof (char));
	strcpy(httpbuf->headers[i], header);
	return 0;
}

int httpbuf_modheader(httpbuf_t *httpbuf, const char *field, const char *value) {
	int idx = httpbuf_findheader(httpbuf, field);
	if (idx < 0) return -1;
	size_t flen = strlen(field);
	size_t nlen = strlen(value) + flen + 5;
	if (nlen > strlen(httpbuf->headers[idx])) {
		httpbuf->headers[idx] = realloc(httpbuf->headers[idx], nlen);
	}
	strcpy(&httpbuf->headers[idx][flen+2], value);
	strcat(&httpbuf->headers[idx][flen+2], "\r\n");
	return 0;
}

void httpbuf_free(httpbuf_t *httpbuf) {
    if (httpbuf) {
        httpbuf_freeheaders(httpbuf);
        if (httpbuf->data) free(httpbuf->data);
        free(httpbuf);
    }
}


static int httpbuf_findheader(httpbuf_t *httpbuf, const char *field) {
	for (size_t i=0;httpbuf->headers[i]!=NULL;i++) {
		if (strncasecmp(httpbuf->headers[i], field, strlen(field)) == 0) {
			return i;
		}
	}
	return -1;
}

static void httpbuf_freeheaders(httpbuf_t *httpbuf) {
	for (size_t idx=0;httpbuf->headers[idx]!=NULL && idx < MAXHEADERS;idx++) 
		free(httpbuf->headers[idx]);
}


