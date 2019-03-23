#include "../csapp.h"

int main() {
    char *buf;
    char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
    int n1 = 0, n2 = 0;

    if ((buf=getenv("QUERY_STRING")) != NULL) {
        char *delim = strchr(buf, '&');
        *delim++ = '\0';
        strcpy(arg1, buf);
        strcpy(arg2, delim);
        n1 = atoi(arg1);
        n2 = atoi(arg2);
    }

    // printf("QUERY_STRING=%s\n", buf);
    sprintf(content, "Welcome to adder.com: ");
    sprintf(content, "%sTHE Internet addition portal.\r\n<p>", content);
    sprintf(content, "%sThe answer is %d + %d = %d\r\n<p>", content, n1, n2, n1+n2);
    sprintf(content, "%sThanks for visiting.\r\n", content);

    printf("Connection: close\r\n");
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-type: text/html\r\n\r\n");
    printf("%s", content);
    fflush(stdout);

    return 0;
}