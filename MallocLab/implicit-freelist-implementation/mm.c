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

#define DEBUG

// Macros
#define FREE  0
#define ALLOC 1

#define DWORD 8
#define SWORD 4

#define CHUNKSIZE 4096

// Operations
#define PACK(size, alloc) ((size&(~0x7))|(alloc))

#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p)=(val))

#define GET_SIZE(p) (GET(p)&(~0x7))
#define GET_ALLOC(p) (GET(p)&0x1)

#define HDRP(bp) ((char*)(bp)-SWORD)
#define FTRP(bp) ((char*)(bp)+GET_SIZE(HDRP(bp))-DWORD)

#define NEXTBLK(bp) ((char*)(bp)+GET_SIZE(HDRP(bp)))
#define PREVBLK(bp) ((char*)(bp)-GET_SIZE(HDRP(bp)-SWORD))

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

/*
 *                      declaration                         
 */

void fill_mem(char *const bp, size_t size);
char *mm_new_chunck(size_t size);
char *mm_search(size_t size);
void mm_place(char *bp, size_t size);
char *mm_coalesce(char *p);

int mm_checker(const char *chk_msg);

/* 
 * mm_init - initialize the malloc package.
 * return 0 when successfully init, otherwise -1.
 */
int mm_init(void)
{
    mem_init();
    char *ptr;
    if ((ptr=(char*)mem_sbrk(CHUNKSIZE)) == NULL) {
        return -1;
    }
#ifdef DEBUG
    fill_mem(ptr, CHUNKSIZE);
#endif
    PUT(ptr, 0);
    ptr+=SWORD; 
    PUT(ptr, PACK(2*SWORD, ALLOC));
    ptr+=SWORD;
    PUT(ptr, PACK(2*SWORD, ALLOC));
    ptr+=SWORD;
    PUT(ptr, PACK(CHUNKSIZE - 4 * SWORD, FREE));
    PUT(FTRP(ptr+SWORD), PACK(CHUNKSIZE - 4 * SWORD, FREE));
    ptr = mem_heap_hi() - 3;
    PUT(ptr, PACK(0, ALLOC));
#ifdef DEBUG
    mm_checker("INIT CHECK");
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
    char *p;
    if ((p=mm_search(size)) == NULL) {
#ifdef DEBUG
        printf("Not found.\n");
#endif
        p = mm_new_chunck(size);
    } 
    mm_place(p+SWORD, size);
#ifdef DEBUG
//    mm_checker("MALLOC CHECK");
#endif
    return p+SWORD;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *p)
{
    char *hdr = HDRP(p);
    PUT(hdr, PACK(GET_SIZE(hdr), FREE));
    PUT(FTRP(p), PACK(GET_SIZE(hdr), FREE));
    char *prev_p = PREVBLK(p);
    char *next_p = NEXTBLK(p);
    if (GET_ALLOC(HDRP(prev_p)) == FREE) {
        hdr = mm_coalesce(hdr);
    }
    if (GET_ALLOC(HDRP(next_p)) == FREE) {
        hdr = mm_coalesce(HDRP(next_p));
    }
//    mm_checker("FREE CHECKER");
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *bp, size_t size)
{
    size_t align_size = (size + 2 * DWORD - 1) / DWORD * DWORD;
    char *next_bp = NEXTBLK(bp);
    size_t now_size = GET_SIZE(HDRP(bp));
    size_t next_size = GET_SIZE(HDRP(next_bp));
    if (size < now_size) {
        if (align_size < now_size - 4 * SWORD) {
            size_t new_size = now_size - align_size;
#ifdef DEBUG
            PUT(FTRP(bp), 0xAAAAAAAA);
#endif
            PUT(HDRP(bp), PACK(align_size, ALLOC));
            PUT(FTRP(bp), PACK(align_size, ALLOC));
            next_bp = NEXTBLK(bp);
            PUT(HDRP(next_bp), PACK(new_size, ALLOC));
            PUT(FTRP(next_bp), PACK(new_size, ALLOC));
            mm_free(next_bp);
        }
        mm_checker("REALLOC CHECKER - SMALLER");
        return bp;
    } else if (GET_ALLOC(next_bp) == FREE
        && next_size + now_size >= align_size + 2 * SWORD) {
#ifdef DEBUG
        PUT(FTRP(bp), 0xAAAAAAAA);
        PUT(HDRP(next_bp), 0xAAAAAAAA);
#endif
        size_t new_size = now_size + next_size - align_size;
        PUT(HDRP(bp), PACK(align_size, ALLOC));
        PUT(FTRP(bp), PACK(align_size, ALLOC));
        next_bp = NEXTBLK(bp);
        PUT(HDRP(next_bp), PACK(new_size, FREE));
        PUT(FTRP(next_bp), PACK(new_size, FREE));
        mm_checker("REALLOC CHECK - ORIGIN");
        return bp;
    } else {
        char *new_bp = mm_malloc(size);
        size_t cp_size = size<now_size?size-DWORD:now_size-DWORD;
        strncpy(new_bp, bp, cp_size);
        mm_free(bp);
        mm_checker("REALLOC CHECK - NEW");
        return new_bp;
    }
}

char *mm_coalesce(char *p) {
    char *bp = p + SWORD;
#ifdef DEBUG
    assert (GET_ALLOC(p) == FREE && GET_ALLOC(HDRP(PREVBLK(bp))) == FREE);
#endif
    size_t size = GET_SIZE(p) + GET_SIZE(HDRP(PREVBLK(bp)));
    printf("size=%u\n", size);
    char *prev_blk = PREVBLK(bp);
#ifdef DEBUG
    PUT(FTRP(prev_blk), 0xAAAAAAAA);
#endif
    PUT(HDRP(prev_blk), PACK(size, FREE));
    PUT(FTRP(bp), PACK(size, FREE));
#ifdef DEBUG
    PUT(HDRP(bp), 0xAAAAAAAA);
#endif
    return HDRP(prev_blk);
}

char *mm_new_chunck(size_t size) {
    size = size <= CHUNKSIZE-SWORD?CHUNKSIZE:(size/CHUNKSIZE+1)*CHUNKSIZE;
    char *p = mem_sbrk(size);
    if (p == NULL) {
        fprintf(stderr, "sbrk error.\n");
        exit(1);
    }
#ifdef DEBUG
    fill_mem(p, size);
#endif
    char *hdr = HDRP(p); 
//    printf("p=%p, hdr=%p, size=%u\n", p, hdr, size);
    PUT(hdr, PACK(size, FREE));
    PUT(FTRP(p), PACK(size, FREE));
    PUT(FTRP(p)+SWORD, PACK(0, ALLOC));
    char *prev_blk = PREVBLK(p);
    if (GET_ALLOC(HDRP(prev_blk)) == FREE) {
//        printf("Coalecse.\n");
        hdr = mm_coalesce(hdr);
    }
    return hdr;
}

char *mm_search(size_t size) {
    char *start = mem_heap_lo() +4 * SWORD;
    char *end = mem_heap_hi()+1;
    while (start != end) {
        char *hdr = HDRP(start);
        if (GET_ALLOC(hdr) == FREE && size < GET_SIZE(hdr)) {
            return hdr;
        } else {
            start = NEXTBLK(start);
        }
    }

    return NULL;
}

void mm_place(char *bp, size_t size) {
    assert (GET_ALLOC(HDRP(bp)) == FREE);
    size_t bigsz = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, ALLOC));
    PUT(FTRP(bp), PACK(size, ALLOC));
    char *next_blk = NEXTBLK(bp);
    PUT(HDRP(next_blk), PACK((bigsz-size), FREE));
    PUT(FTRP(next_blk), PACK((bigsz-size), FREE));
}

/*************************************************************
 *************************************************************/

void mm_print_check(const char *msg) {
    fprintf(stderr, "mm_checker: %s\n", msg);
}

void fill_mem(char *const bp, size_t size) {
    for (char *ptr=bp;ptr<bp+size;ptr++) {
        *ptr = 0xAA;
    }
}

int mm_checker(const char *msg) {
    
#ifndef DEBUG
    return 0;
#endif
    
    printf("%s\nheap_lo=%p, heap_hi=%p, heap_size=%u\n", 
        msg, mem_heap_lo(), mem_heap_hi(), mem_heapsize());

    char* heap_start = mem_heap_lo();
    char* heap_end;
    
    // padding word
    if (*heap_start != 0) {
        mm_print_check("padding word is not zero");
        return 0;
    }

    heap_start += SWORD;
    // prologue block
    if (GET_SIZE(heap_start) != DWORD || GET_ALLOC(heap_start) != 1) {
        mm_print_check("prologue block error");
        return 0;
    }
    heap_start += SWORD;
    if (GET_SIZE(heap_start) != DWORD || GET_ALLOC(heap_start) != 1) {
        mm_print_check("prologue block error");
        return 0;
    }
    
    // epilogue block
    heap_end = mem_heap_hi() - 3;
    if (GET_SIZE(heap_end) != 0 || GET_ALLOC(heap_end) != 1) {
        mm_print_check("epilogue block error");
        return 0;
    } 

    printf("padding block, prologue block and epilogue block check pass.\n");

    // print heap - PLEASE set every byte of payload to 0xAA!!!
    struct {
        char *address;
        char val;
        size_t size;
        int status;
    } heap_info[1024];
    int n = 0;
    heap_start = mem_heap_lo()+3*SWORD;
    heap_end = mem_heap_hi()-3;
    for (char *ptr=heap_start;ptr!=heap_end;ptr+=SWORD) {
        if ((unsigned char)*ptr != 0xAA) {
            heap_info[n].address = ptr;
            heap_info[n].val = *(unsigned int*)ptr;
            heap_info[n].size = GET_SIZE(ptr);
            heap_info[n].status = GET_ALLOC(ptr);
            n ++;
        }
    }
    for (int i=0;i<n;i++) {
        fprintf(stderr, "  -----------------\n");
        fprintf(stderr, "  |addr:%10p|\n", heap_info[i].address);
        fprintf(stderr, "  | val:%10u|\n", heap_info[i].val);
        fprintf(stderr, "  |size:%10u|\n", heap_info[i].size);
        fprintf(stderr, "  |status:%8s|\n", heap_info[i].status==FREE?"FREE":"ALLOC");
        fprintf(stderr, "  -----------------\n");
        if (i % 2 == 1) fprintf(stderr, "*********************\n");
        // fprintf(stderr, "*****************\n");
    }

    // check allocated size consistency
    int size = 4 * SWORD;
    heap_start = mem_heap_lo() + 3 * SWORD;
    heap_end = mem_heap_hi()-3;
    for (char *ptr=heap_start;ptr<=heap_end;ptr+=GET_SIZE(ptr)) {
        if (GET_SIZE(ptr) == 0) break;
        size += GET_SIZE(ptr);
    }
    if (size < mem_heapsize()) {
        mm_print_check("size inconsistent, size sum over");
        return 0;
    } else if (size < mem_heapsize()) {
        mm_print_check("size inconsistent, size sum below");
        return 0;
    }
    printf("size consistency check pass.\n");

    // block overlap


    // coalesce check

}














