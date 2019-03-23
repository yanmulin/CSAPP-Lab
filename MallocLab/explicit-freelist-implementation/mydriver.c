#include <stdio.h>
#include <assert.h>

#define SWORD 4

#include "mm.h"
#include "memlib.h"

int main() {
    mm_init();
    char *p1 = mm_malloc(3);
    char *p2 = mm_malloc(8888);
    char *p3 = mm_malloc(41);
    p2 = mm_realloc(p2, 4000);
    mm_free(p2);
    p1 = mm_realloc(p1, 8889);
//    p3 = mm_realloc(p3, 38);
//    p3 = mm_realloc(p3, 500);
    return 0;
}
