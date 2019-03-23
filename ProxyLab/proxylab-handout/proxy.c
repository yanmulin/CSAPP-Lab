#include "csapp.h"
#include "sbuf.h"
#include "timestamp.h"
#include "ncache.h"
#include "httpbuf.h"

#define MAXTHREAD 10

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3";
static sbuf_t sbuf;
static tsbuf_t tsbuf;
static ncache_t ncache;

int add_header(char *hdrs[], size_t n, const char *header);
void send_headers(int fd, char *hdrs[], size_t n);

int log_print(char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int ret = 0;
	ret = vprintf(fmt, ap);
	va_end(ap);
	fflush(stdout);
	return ret;
}

void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
	httpbuf_t httpbuf;
    char body[MAXLINE], buf[MAXLINE];
	size_t body_len;
    snprintf(body, MAXLINE,  "<html><title>Tiny Error</title>");
    snprintf(buf, MAXLINE, "<body bgcolor=\"ffffff\">%s: %s\r\n", errnum, shortmsg);
	strcat(body, buf);
    snprintf(buf, MAXLINE, "<p>%s: %s</body><html>", longmsg, cause);
	strcat(body, buf);
    snprintf(buf, MAXLINE, "<hr><em>The Tiny Web server</em>\r\n");
	strcat(body, buf);

	body_len = strlen(body);

	snprintf(buf, MAXLINE,  "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
	httpbuf_addheader(&httpbuf, "Content-type: text/html\r\n");
	snprintf(buf, MAXLINE, "Content-length: %d\r\n\r\n", (int)body_len);
	httpbuf_addheader(&httpbuf, buf);
	httpbuf.data = body;
	httpbuf.data_len = strlen(body);
	httpbuf_write(&httpbuf, fd);
}

int parse_url(char *url, char *host, char *port, char *uri) {
	char *st = url;
	char *p = url;
	if ((p=strstr(st, "://")) != NULL) {
		st = p+3;
	}
	if ((p=strchr(st, ':')) != NULL) {
		*p++ = '\0';
		strcpy(host, st);
		st = p;
		p = strchr(st, '/');
		if (p) *p++ = '\0';
		strcpy(port, st);
	} else {
		p = strchr(st, '/');
		if (p) *p++ = '\0';
		strcpy(host, st);	
		strcpy(port, "80");
	}
	strcpy(uri, "/");
	if (p && *p) strcat(uri, p);
	return 0;
}

httpbuf_t *connect2url(rio_t *crp, char *url, char *method) {
	int sfd, cfd;
	rio_t srio;
	char buf[MAXLINE], port[1024], host[1024], uri[1024];

	cfd = crp->rio_fd;
	
	if (parse_url(url, host, port, uri) < 0) {
		client_error(cfd, url, "400", "Not found", "illegal url");
		return NULL;
	}

	sfd = Open_clientfd(host, port);
	log_print("CONNECTED TO SERVER:(%s, %s)\n", host, port);


	httpbuf_t rchttp, *snhttp = NULL;
	httpbuf_read(&rchttp, crp);
	httpbuf_modheader(&rchttp, "Connection", "close");
	httpbuf_modheader(&rchttp, "Proxy-Connection", "close");
	httpbuf_modheader(&rchttp, "User-Agent", user_agent_hdr);
	
	snprintf(buf, MAXLINE,  "%s %s %s\r\n", method, uri, "HTTP/1.0");
	rio_writen(sfd, buf, strlen(buf));
	httpbuf_write(&rchttp, sfd);

	snhttp = httpbuf_new();
	Rio_readinitb(&srio, sfd);

	ssize_t rc = rio_readlineb(&srio, buf, MAXLINE);
	rio_writen(cfd, buf, rc);

	httpbuf_readheaders(snhttp, &srio);
	httpbuf_write(snhttp, cfd);

	httpbuf_readdata(snhttp, &srio);
	httpbuf_writedata(snhttp, cfd);

	httpbuf_writeheaders(snhttp, STDOUT_FILENO);
	httpbuf_writedata(snhttp, STDOUT_FILENO);
	log_print("SEND %ld BYTES DATA.\n", snhttp->data_len);
	
	close(sfd);

	return snhttp;
}

void doit(int clientfd) {
	rio_t rio;
	char buf[MAXLINE];
	char method[128], url[MAXLINE], version[1024];

	Rio_readinitb(&rio, clientfd);

	Rio_readlineb(&rio, buf, MAXLINE);
	sscanf(buf, "%s %s %s", method, url, version);
	log_print("RECEIVED: %s %s %s\n", method, url, version);

	if (strcasecmp(method, "CONNECT") == 0) {
		strcpy(method, "GET");
	} else if (strcasecmp(method, "HEAD") == 0) {
		strcpy(method, "GET");
	} else {
	}

	// connect2url(&rio, url, method);

	nc_node_t *node = ncache_search(&ncache, url);

	if (node == NULL || node->data == NULL) {
		node = nc_node_new(url, timestamp_now(&tsbuf));
		node->data = connect2url(&rio, url, method);
	} else {
		char *msg= "HTTP/1.0 200 OK\r\n";
		rio_writen(clientfd, msg, strlen(msg));
		httpbuf_write(node->data, clientfd);
	}
	
	ncache_clear(&ncache, node);
}

void *thread(void *arg) {
	int connfd;
	pthread_detach(pthread_self());
	while (1) {
		connfd = sbuf_remove(&sbuf);
		doit(connfd);
		Close(connfd);
	}
	return NULL;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./proxy <port>\n");
		exit(0);
	}

	Signal(SIGPIPE, SIG_IGN);

	sbuf_init(&sbuf, 10);
	timestamp_init(&tsbuf);
	ncache_init(&ncache);

	pthread_t tid;
	for (int i=0;i<MAXTHREAD;i++) 
		pthread_create(&tid, NULL, thread, NULL);

	int listenfd = Open_listenfd(argv[1]);
	struct sockaddr_storage client_addr;
	socklen_t client_len;
	char client_hostname[MAXLINE], client_port[MAXLINE];	
	while (1) {
		client_len = sizeof(client_addr);
		int connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
		getnameinfo((SA *)&client_addr, client_len, client_hostname, MAXLINE, client_port, MAXLINE, NI_NUMERICSERV);
		log_print("CONNECTED: (%s, %s)\n", client_hostname, client_port);
		sbuf_insert(&sbuf, connfd);
	}
    return 0;
}
