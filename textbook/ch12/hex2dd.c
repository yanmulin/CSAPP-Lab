#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void print_error() {
    fprintf(stderr, "input error.\n");
}

int main(int argc, char **argv) {
    if (argc == 1) return 0;

    unsigned hex = strtol(argv[1], NULL, 16);
    if (strncmp(argv[1], "0x", 2) == 0 && strlen(argv[1]) != 10) {
        print_error();
        return 1;
    } else if (strncmp(argv[1], "0x", 2) != 0 && strlen(argv[1]) != 8) {
        print_error();
        return 1;
    }
    for (int i=0;i<4;i++) {
        printf("%u", hex >> 24 & 0xff);
        hex = hex << 8;
        if (i != 3) printf(".");
    }
    printf("\n");
    return 0;
}