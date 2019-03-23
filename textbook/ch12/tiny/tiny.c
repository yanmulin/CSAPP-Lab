#include "csapp.h"

#define STATIC 0
#define DYNAMIC 1

void client_error(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
    char body[MAXLINE], buf[MAXLINE];
    sprintf(body, "<html><title>Tiny Error</title>");
    sprintf(body, "%s<body bgcolor=\"ffffff\">%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s</body><html>", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));

    Rio_writen(fd, body, strlen(body));
}

void get_filetype(char *filename, char *filetype) {
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif")) 
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".png")) 
        strcpy(filetype, "image/png");
    else if (strstr(filename, ".jpg")) 
        strcpy(filetype, "image/jpg");
    else 
        strcpy(filetype, "text/plain");
}

void read_headers(rio_t *rp) {
    char buf[MAXLINE];
    rio_readlineb(rp, buf, MAXLINE);
    while (strcmp(buf, "\r\n")) {
        printf("%s", buf);
        rio_readlineb(rp, buf, MAXLINE);
    }
}

void serve_static(int fd, char *filename, size_t filesize) {
    char filetype[MAXLINE];
    get_filetype(filename, filetype);

    char headers[MAXLINE];
    sprintf(headers, "HTTP/1.0 200 OK\r\n");
    sprintf(headers, "%sServer: Tiny Web Server\r\n", headers);
    sprintf(headers, "%sConnection: Close\r\n", headers);
    sprintf(headers, "%sContent-Type: %s\r\n", headers, filetype);
    sprintf(headers, "%sContent-Length: %d\r\n\r\n", headers, (int)filesize);
    Rio_writen(fd, headers, strlen(headers));
    printf("Respond headers:\n");
    printf("%s", headers);

    int srcfd = Open(filename, 0, O_RDONLY);
    void *srcp = Mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    Rio_writen(fd, srcp, filesize);
    Munmap(srcp, filesize);
}

void serve_dynamic(int fd, char *filename, char *cgiargs) {
    char *emptylist[] = {NULL};
    char buf[MAXLINE];
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Server: Tiny Web Server\r\n");
    Rio_writen(fd, buf, strlen(buf));
    
    pid_t pid = Fork();
    if (pid == 0) {
        if (setenv("QUERY_STRING", cgiargs, 1) < 0) {
            fprintf(stderr, "setenv error.\n");
            exit(0);
        }   
        dup2(fd, STDOUT_FILENO);
        Execve(filename, emptylist, environ);
    }
    Wait(NULL);
}

int parse_uri(char *uri, char *filename, char *cgiargs) {
    strcpy(filename, ".");
    if (!strstr(uri, "cgi-bin")) {
        strcpy(cgiargs, "");
        strcat(filename, uri);
        if (filename[strlen(uri)] == '/') {
            strcat(filename, "home.html");
        }
        return STATIC;
    } else {
        char *delim = strchr(uri, '?');
        if (delim) {
            *delim++ = '\0';
            strcpy(cgiargs, delim);
        } else {
            strcpy(cgiargs, "");
        }
        strcat(filename, uri);
        return DYNAMIC;
    }
}

void doit(int connfd) {
    rio_t rio;
    char buf[MAXLINE];

    rio_readinitb(&rio, connfd);
    rio_readlineb(&rio, buf, MAXLINE);
    printf("TEST:%s", buf);
    
    char method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    sscanf(buf, "%s %s %s", method, uri, version);

    if (strcasecmp(method, "GET")) {
        client_error(connfd, method, "501", "Not implemented",
                    "Tiny does not implemented this method");
        return;
    }

    read_headers(&rio);

    char filename[MAXLINE], cgiargs[MAXLINE];
    struct stat filestat;
    int is_static = parse_uri(uri, filename, cgiargs);
    if (stat(filename, &filestat) < 0) {
        client_error(connfd, filename, "404", "Not found",
                    "Tiny couldn't find this file");
        return;
    }
    if (is_static == STATIC) {
        if (!S_ISREG(filestat.st_mode) || !(S_IRUSR & filestat.st_mode)) {
            client_error(connfd, filename, "403", "Forbidden", 
                        "Tiny Couldn't run the CGI program");
            return;
        }
        serve_static(connfd, filename, filestat.st_size);
    } else {
        if (!S_ISREG(filestat.st_mode) || !(S_IEXEC & filestat.st_mode)) {
            client_error(connfd, filename, "403", "Forbidden",
                        "Tiny couldn't run the CGI program");
            return;
        }
        serve_dynamic(connfd, filename, cgiargs);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: tinyweb <port>\n");
        exit(0);
    }

    char *port = argv[1];
    int listenfd = Open_listenfd(port);
    struct sockaddr_storage client_addr;
    socklen_t client_len;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    while (1) {
        client_len = sizeof(struct sockaddr_storage);
        int connfd = Accept(listenfd, (SA *)&client_addr, &client_len);
        getnameinfo((SA *)&client_addr, client_len, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected by (%s, %s)\n", client_hostname, client_port);
        doit(connfd);
        Close(connfd);
    }
}