/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 * 
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

//#define DEBUG

// Macros
#define FREE  0
#define ALLOC 1

#define DWORD 8
#define SWORD 4

#define CHUNCKSIZE 4096

// Operations
#define PACK(size, alloc) ((size&~0x7)|alloc)

#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p)=(val))

#define GET_SIZE(p) (GET(p)&~0x7)
#define GET_ALLOC(p) (GET(p)&0x1)

#define HDRP(bp) ((char*)(bp)-SWORD)
#define FTRP(bp) ((char*)(bp)+GET_SIZE(HDRP(bp))-DWORD)

#define NEXTBLK(bp) ((char*)(bp)+GET_SIZE(HDRP(bp)))
#define PREVBLK(bp) ((char*)(bp)-GET_SIZE(bp-DWORD))

team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

typedef struct FreeNode {
    struct FreeNode *pred;
    struct FreeNode *succ;
} FreeNode;

static FreeNode firstNode = {NULL, NULL};

#define NODE(p) (*(FreeNode *)((char *)(p)+SWORD))

int mm_checker(const char *msg);
void mm_fill(char *p, size_t size);

void *mm_search(size_t);
void *mm_new_chunck(size_t);
void mm_place(char *, size_t);
void *mm_coalesce(char *);

/* 
 * mm_init - initialize the malloc package.
 * return 0 when successfully init, otherwise -1.
 */
int mm_init(void)
{
    mem_init();
    char *p;
    if ((p=mem_sbrk(CHUNCKSIZE)) == NULL) {
        return -1;
    }
#ifdef DEBUG
    mm_fill((char *)p, CHUNCKSIZE);
#endif
    PUT(p, 0);
    p+=SWORD;
    PUT(p, PACK(DWORD, ALLOC));
    p+=SWORD;
    PUT(p, PACK(DWORD, ALLOC));
    p+=SWORD;
    PUT(p, PACK((CHUNCKSIZE - 4 * SWORD), FREE));
    PUT(FTRP(p+SWORD), PACK((CHUNCKSIZE - 4 * SWORD), FREE));
    firstNode.succ = &NODE(p);
    NODE(p).pred = &firstNode;
    NODE(p).succ = NULL;
    p = mem_heap_hi() - 3;
    PUT(p, PACK(0, ALLOC));
#ifdef DEBUG
    mm_checker("INIT");
#endif
    return 0;
}

/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
    size = (size + 2 * DWORD - 1) / DWORD * DWORD;
    size = size >= 4*SWORD?size:4*SWORD;
    char *p;
    if ((p=mm_search(size)) == NULL) {
        p = mm_new_chunck(size);
    }
    if (p == NULL) return NULL;
    mm_place(p, size);
#ifdef DEBUG
    mm_checker("MALLOC");
#endif
    return p + SWORD;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp)
{
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, FREE));
    PUT(FTRP(bp), PACK(size, FREE));
    NODE(HDRP(bp)).succ = firstNode.succ;
    NODE(HDRP(bp)).pred = &firstNode;
    if (firstNode.succ != NULL) {
        firstNode.succ->pred = (FreeNode *)bp;
    }
    firstNode.succ = (FreeNode *)bp;
    if (GET_ALLOC(HDRP(NEXTBLK(bp))) == FREE) {
        mm_coalesce(HDRP(NEXTBLK(bp)));
    }
    if (GET_ALLOC(HDRP(PREVBLK(bp))) == FREE) {
        mm_coalesce(HDRP(bp));
    }
#ifdef DEBUG
    mm_checker("FREE");
#endif
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size) {
    size_t origin_size = size;
    size = (size + 2*DWORD - 1)/DWORD*DWORD;
    size = size<2*DWORD?2*DWORD:size;
    size_t now_size = GET_SIZE(HDRP(bp));
    char *next_bp = NEXTBLK(bp);
    size_t next_size = GET_SIZE(HDRP(next_bp));
    if (now_size > size) {
        if (now_size - size > 2 * DWORD) {
#ifdef DEBUG
            PUT(FTRP(bp), 0x55555555);
#endif
            PUT(HDRP(bp), PACK(size, ALLOC));
            PUT(FTRP(bp), PACK(size, ALLOC));
            next_bp = NEXTBLK(bp);
            PUT(HDRP(next_bp), PACK((now_size-size), FREE));
            PUT(FTRP(next_bp), PACK((now_size-size), FREE));
            NODE(HDRP(next_bp)).pred = &firstNode;
            NODE(HDRP(next_bp)).succ = firstNode.succ;
            if (firstNode.succ != NULL) {
                firstNode.succ->pred = (FreeNode *)next_bp;
            }
            firstNode.succ = (FreeNode *)next_bp;
        }
    } else if (GET_ALLOC(HDRP(next_bp)) == FREE && next_size + now_size >= size) {

        NODE(HDRP(next_bp)).pred->succ = NODE(HDRP(next_bp)).succ;
        if (NODE(HDRP(next_bp)).succ != NULL) {
            NODE(HDRP(next_bp)).succ->pred = NODE(HDRP(next_bp)).pred;
        }
#ifdef DEBUG
        PUT(HDRP(next_bp)+SWORD, 0x55555555);
        PUT(HDRP(next_bp)+2*SWORD, 0x55555555);
        PUT(FTRP(bp), 0x55555555);
        PUT(HDRP(next_bp), 0x55555555);
#endif
        if (next_size + now_size - size < 2 * DWORD) {
            PUT(HDRP(bp), PACK((next_size + now_size), ALLOC));
            PUT(FTRP(bp), PACK((next_size + now_size), ALLOC));
        } else {
            size_t new_size = next_size + now_size - size;
            PUT(HDRP(bp), PACK(size, ALLOC));
            PUT(FTRP(bp), PACK(size, ALLOC));
            next_bp = NEXTBLK(bp);
            PUT(HDRP(next_bp), PACK(new_size, FREE));
            PUT(FTRP(next_bp), PACK(new_size, FREE));
            NODE(HDRP(next_bp)).pred = &firstNode;
            NODE(HDRP(next_bp)).succ = firstNode.succ;
            if (firstNode.succ != NULL) {
                firstNode.succ->pred = &NODE(HDRP(next_bp));
            }
            firstNode.succ = &NODE(HDRP(next_bp));
        }
    } else {
        char *origin_bp = bp;
        bp = mm_malloc(origin_size);
        strncpy(origin_bp, bp, origin_size);
        mm_free(origin_bp);
    }
#ifdef DEBUG
    mm_checker("REALLOC");
#endif
    return bp;
}


/*
 * Helper Functions
 */
void *mm_search(size_t size) {
    FreeNode *node;
    for (node=firstNode.succ;node!=NULL;node=node->succ) {
        if (GET_SIZE(HDRP(node)) > size) {
            return HDRP(node);
        }
    }
    return NULL;
}

void *mm_new_chunck(size_t size) {
    size_t chunck_num = size<CHUNCKSIZE?1:size/CHUNCKSIZE+1;
    char *p;
    if ((p=mem_sbrk(chunck_num * CHUNCKSIZE)) == NULL) {
        return NULL;
    }
#ifdef DEBUG
    mm_fill(p, chunck_num * CHUNCKSIZE);
#endif
    char *hdr = p - SWORD;
    PUT(HDRP(p), PACK(chunck_num*CHUNCKSIZE, FREE));
    PUT(FTRP(p), PACK(chunck_num*CHUNCKSIZE, FREE));
    NODE(hdr).pred = &firstNode;
    NODE(hdr).succ = firstNode.succ;
    if (firstNode.succ != NULL) {
        firstNode.succ->pred = &NODE(hdr);
    }
    firstNode.succ = &NODE(hdr);
    char *next_bp = NEXTBLK(p);
    PUT(HDRP(next_bp), PACK(0, ALLOC));
//    fprintf(stderr, "p=%p, prev_p=%p\n", p, PREVBLK(p));
    if (GET_ALLOC(HDRP(PREVBLK(p))) == FREE) {
//        fprintf(stderr, "COALESCE\n");
        hdr = mm_coalesce(hdr);
    }
    return hdr;
}

void mm_place(char *p, size_t size) {
    assert (GET_ALLOC(p) == FREE && GET_SIZE(p) >= size);
    size_t bigsz = GET_SIZE(p);
    
    NODE(p).pred->succ = NODE(p).succ;
    if (NODE(p).succ != NULL) {
        NODE(p).succ->pred = NODE(p).pred;
    }
#ifdef DEBUG
    PUT(p+SWORD, 0x55555555);
    PUT(p+2*SWORD, 0x55555555);
#endif
    
    if (bigsz - size >= DWORD + sizeof(FreeNode)) {
        char *next_bp = p + size + SWORD;
        PUT(p, PACK(size, ALLOC));
        PUT(FTRP(p+SWORD), PACK(size, ALLOC));
        PUT(HDRP(next_bp), PACK((bigsz-size), FREE));
        PUT(FTRP(next_bp), PACK((bigsz-size), FREE));
        NODE(HDRP(next_bp)).pred = &firstNode;
        NODE(HDRP(next_bp)).succ = firstNode.succ;
        if (firstNode.succ != NULL) {
            firstNode.succ->pred = (FreeNode *)next_bp;
        }
        firstNode.succ = (FreeNode *)next_bp;
    } else {
        PUT(p, PACK(bigsz, ALLOC));
        PUT(FTRP(p+SWORD), PACK(bigsz, ALLOC));
    }
}

void *mm_coalesce(char *p) {
    char *prev_bp = PREVBLK(p+SWORD);
    size_t new_size = GET_SIZE(HDRP(prev_bp)) + GET_SIZE(p);
    assert (GET_ALLOC(p) == FREE && GET_ALLOC(HDRP(prev_bp)) == FREE);
    NODE(p).pred->succ = NODE(p).succ;
    if (NODE(p).succ != NULL) {
        NODE(p).succ->pred = NODE(p).pred;
    }
#ifdef DEBUG
    PUT(p+SWORD, 0x55555555);
    PUT(p+2*SWORD, 0x55555555);
#endif

#ifdef DEBUG
    PUT(p, 0x55555555);
    PUT(FTRP(prev_bp), 0x55555555);
#endif
    PUT(HDRP(prev_bp), PACK(new_size, FREE));
    PUT(FTRP(prev_bp), PACK(new_size, FREE));
    
    return HDRP(prev_bp);
}


/*
 * Checker Functions
 */
void mm_fill(char *p, size_t size) {
    for (size_t i=0;i<size;i++) {
        *(p+i) = 0x55;
    }
}

void mm_print_check(const char *msg) {
    fprintf(stderr, "mm_checker: %s\n", msg);
}

int mm_checker(const char *msg) {
     fprintf(stderr, "%s\n", msg);
     
     unsigned int *heap_start = mem_heap_lo();
     unsigned int *heap_end = mem_heap_hi() - 3;

     // padding word
     if (*heap_start != 0) {
         mm_print_check("padding word is not zero");
         return 0;
     }

     heap_start ++;
     // prologue block
     if (GET_SIZE(heap_start) != DWORD || GET_ALLOC(heap_start) != ALLOC) {
         mm_print_check("prologue block error");
         return 0;
     }
     heap_start ++;
     if (GET_SIZE(heap_start) != DWORD || GET_ALLOC(heap_start) != ALLOC) {
         mm_print_check("prologue block error");
         return 0;
     }

     // epilogue block
     if (GET_SIZE(heap_end) != 0 || GET_ALLOC(heap_end) != ALLOC) {
         mm_print_check("epilogue block error");
         return 0;
     }

     // print heap - PLEASE set every byte of payload to 0x55!!!
     struct {
         char *address;
         size_t size;
         unsigned long val;
         int status;
     } heap_info[1024];
     int n = 0;
     heap_start = mem_heap_lo() + 3 * SWORD;
     heap_end = mem_heap_hi() - 3;
     for (char *ptr=(char*)heap_start;ptr<(char *)heap_end;ptr+=SWORD) {
         if ((unsigned char)*ptr != 0x55) {
             heap_info[n].address = ptr;
             heap_info[n].size = GET_SIZE(ptr);
             heap_info[n].val = *(unsigned long *)ptr;
             heap_info[n].status = GET_ALLOC(ptr);
             if (n % 2 == 0 && GET_ALLOC(ptr) == FREE) {
                 ptr += sizeof (FreeNode);
             }
             n++;
         }
     }
     for (int i=0;i<n;i++) {
         fprintf(stderr, "-----------------\n");
         fprintf(stderr, "|addr:%10p|\n", heap_info[i].address);
         fprintf(stderr, "|val :0x%8lx|\n", heap_info[i].val);
         fprintf(stderr, "|size:%10u|\n", heap_info[i].size);
         fprintf(stderr, "|status:%8s|\n", heap_info[i].status==FREE?"FREE":"ALLOC");
         fprintf(stderr, "-----------------\n");
         if (i % 2 == 1) fprintf(stderr, "*****************");
         fprintf(stderr, "*****************\n");
     }

      // check allocated size consistency
    heap_start = mem_heap_lo() + 3 * SWORD;
    heap_end = mem_heap_hi() - 3;
    size_t size = 4 * SWORD;
    for (char *ptr=(char*)heap_start;ptr<(char*)heap_end;ptr+=GET_SIZE(ptr)) {
        if (GET_SIZE(ptr) == 0) break;
        size += GET_SIZE(ptr);
    }
    if (size < mem_heapsize()) {
        mm_print_check("size inconsistent, size sum surpass");
        return 0;
    } else if (size < mem_heapsize()) {
        mm_print_check("size inconsistent, size sum insufficent");
        return 0;
    }
    
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    for (FreeNode *node=firstNode.succ;node!=NULL;node=node->succ) {
        fprintf(stderr, "*****************\n");
        fprintf(stderr, "|addr:%10p|\n", node);
        fprintf(stderr, "|pred:%10p|\n", node->pred);
        fprintf(stderr, "|succ:%10p|\n", node->succ);
        fprintf(stderr, "*****************\n");
    }
    fprintf(stderr, "**********************************\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    return 0;
 }














