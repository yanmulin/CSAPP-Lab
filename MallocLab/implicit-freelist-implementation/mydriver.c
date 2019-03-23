#include <stdio.h>
#include <assert.h>

#define SWORD 4

#include "mm.h"
#include "memlib.h"

extern void mm_checker(const char *);

int main() {
    mm_init();
    char *p1 = mm_malloc(3);
    char *p2 = mm_malloc(3);
    char *p3 = mm_malloc(7);
    mm_checker("MARK #1");
//    mm_free(p1);
    p2 = mm_realloc(p2, 53);
//    mm_free(p1);
//    mm_free(p2);
//    mm_free(p3);
    return 0;
}
