#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_error() {
    fprintf(stderr, "input error");
    exit(1);
}

int main(int argc, char **argv) {
    if (argc == 1) return 0;

    char *p = argv[1];
    int n = 0;
    unsigned hex = 0;
    do {
        unsigned long dd = strtol(p, NULL, 10);
        if (0 > dd || dd > 255) print_error(); 
        if (n++ >= 4) print_error();
        hex = (hex << 8) + dd; 
        p = strchr(p, '.');
    } while (p++ != NULL);    
    printf("0x%x\n", hex);
    return 0;
}