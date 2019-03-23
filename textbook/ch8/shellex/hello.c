#include <stdio.h>
#include "csapp.h"

int main(){
    printf("hello foreground pgid: %d\n", tcgetpgrp(STDIN_FILENO));
    printf("Hello world.\n");
    return 0;
}