#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include "cachelab.h"

#define MAXLINE 1024

unsigned long timestamp = 0;
int hit_count = 0, miss_count = 0, eviction_count = 0;
int global_s, global_E , global_b;
bool verbose_option = false;

enum {INSTRUCTION = 'I', LOAD = 'L', STORE = 'S', MODIFY = 'M'};

typedef unsigned long AddressType;
typedef struct {
    bool isValid;
    AddressType signature;
    char *block;
    unsigned timestamp;
}Line;

typedef Line *Group;
typedef Group *Cache;

Cache cache;

void *Malloc(size_t size) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        fprintf(stderr, "malloc error\n");
        exit(1);
    }
    return ptr;
}

void putMsg(const char *msg, ...) {
    va_list ap;
    va_start(ap, msg);
    if (verbose_option) vprintf(msg, ap);
    va_end(ap);
}

void initalizeCache(int s, int E, int b) {
    global_s = s;
    global_E = E;
    global_b = b;
    int groupSize = 1 << s;
    int lineNum = E;
    int blockSize = 1 << b; 
    cache = (Cache)Malloc(sizeof (Group) * groupSize);
    for (int group = 0;group<groupSize;group ++) {
        cache[group] = (Group)Malloc(sizeof (Line) * lineNum);
        for (int line = 0;line < lineNum;line ++) {
            cache[group][line].isValid = false;
            cache[group][line].block = (char*)Malloc(sizeof (char) * blockSize);
        }
    }
    timestamp ++;
}

void freeCache() {
    int groupSize = 1 << global_s;
    int lineNum = global_E;
    for (int i=0;i<groupSize;i++) {
        for (int j=0;j<lineNum;j++) {
            free(cache[i][j].block);
        }
        free(cache[i]);
    }
    free(cache);
}

int getGroupId(AddressType address) {
    unsigned long mask = ~(~0 << global_s);
    int groupId = (address >> global_b) & mask;
    return groupId;
}

AddressType getSignature(AddressType address) {
    unsigned long mask = ~(~0 << (64 - global_b - global_s));
    AddressType signature = address >> (global_b + global_s) & mask;
    return signature;
}

bool matchLine(int groupId, AddressType signature, int *lineId) {
    Group group = cache[groupId];
    bool found = false;
    for (int i=0;i<global_E;i++) {
        if (group[i].isValid && signature == group[i].signature) {
            found = true;
            *lineId = i;
            break;
        }
    }
    return found;
}

bool anyEmptyLine(int groupId, int *lineId) {
    Group group = cache[groupId];
    bool found = false;
    for (int i=0;i<global_E;i++) {
        if (group[i].isValid == false) {
            *lineId = i;
            found = true;
            break;
        }
    }
    return found;
}

int evictLine(int groupId) {
    Group group = cache[groupId];
    unsigned long min_timestamp = group[0].timestamp;
    int min_idx = 0;
    for (int i=1;i<global_E;i++) {
        if (group[i].timestamp < min_timestamp) {
            min_idx = i;
            min_timestamp = group[i].timestamp;
        }
    }
    return min_idx;
}

void queryCache(AddressType address) {
    
    int groupId = getGroupId(address);
    AddressType signature = getSignature(address);
    int lineId;
    bool success = matchLine(groupId, signature, &lineId);
    if (success) {
        // hit
        hit_count ++;
        cache[groupId][lineId].timestamp = timestamp;
        putMsg("hit ");
        timestamp ++;
        return ;
    } 
    // miss
    putMsg("miss ");
    miss_count ++;
    bool emptyLine = anyEmptyLine(groupId, &lineId);
    if (emptyLine) {
        // hit
        cache[groupId][lineId].isValid = true;
        cache[groupId][lineId].signature = signature;
        cache[groupId][lineId].timestamp = timestamp;
        // printf("");
    } else {
        // eviction
        lineId = evictLine(groupId);
        // printf("0x%lx:%u", (cache[groupId][lineId].signature<<(global_s + global_b)), cache[groupId][lineId].timestamp);
        cache[groupId][lineId].signature = signature;
        cache[groupId][lineId].timestamp = timestamp;
        eviction_count ++;
        putMsg("eviction ");
    }

    timestamp ++;
}

void eval(const char *line) {
    char op;
    AddressType address;
    unsigned size;

    const char *ptr = line;
    while (*ptr == ' ') ptr ++;

    sscanf(ptr, "%c %lx,%d", &op, &address, &size);
    if (op != INSTRUCTION) 
        putMsg("%c %lx,%d ", op, address, size);

    switch (op) {
        case INSTRUCTION:break;
        case LOAD:queryCache(address);break;
        case STORE:queryCache(address);break;
        case MODIFY:queryCache(address);queryCache(address);break;
        default:assert(0);
    }
    if (op != INSTRUCTION) 
        putMsg("\n");
}

void printUsage(const char *cmdline) {
    fprintf(stderr, "Usage: %s [-hv] -s <num> -E <num> -b <num> -t <file>\n", cmdline);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "-h         Print this help message.\n");
    fprintf(stderr, "-v         Optional verbose flag.\n");
    fprintf(stderr, "-s <num>   Number of set index bits.\n");
    fprintf(stderr, "-E <num>   Number of lines per set.\n");
    fprintf(stderr, "-b <num>   Number of block offset bits.\n");
    fprintf(stderr, "-t <file>  Trace file.\n");
    fprintf(stderr, "Examples:\n");
    fprintf(stderr, "linux>  %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", cmdline);
    fprintf(stderr, "linux>  %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", cmdline);
}

void printArgError(const char *cmdline) {
    fprintf(stderr, "%s: Missing required command line argument\n", cmdline);
    printUsage(cmdline);
    exit(0);
}

FILE* optionsParser(int argc, char *argv[]) {
    bool got_s = false, got_E = false, got_b = false;
    int s, E, b;
    FILE *fp = stdin;
    if (argc < 7) 
        printArgError(argv[0]);
    int arg = 1;
    while (arg < argc) {
        if (strcmp(argv[arg], "-h") == 0) {
            printUsage(argv[0]);
            exit(0);
        } else if (strcmp(argv[arg], "-s") == 0) {
            arg ++;
            if ((s=atoi(argv[arg]))==0) 
                printArgError(argv[0]);
            else got_s = true;
        } else if (strcmp(argv[arg], "-E") == 0) {
            arg ++;
            if ((E=atoi(argv[arg])) == 0)
                printArgError(argv[0]);
            else got_E = true;
        } else if (strcmp(argv[arg], "-b") == 0) {
            arg ++;
            if ((b=atoi(argv[arg])) == 0)
                printArgError(argv[0]);
            else got_b = true;
        } else if (strcmp(argv[arg], "-t") == 0) {
            arg ++;
            fp = fopen(argv[arg], "r");
            if (fp == NULL) {
                fprintf(stderr, "%s: No such file or directory.\n", argv[arg]);
                exit(0);
            }
        } else if (strcmp(argv[arg], "-v") == 0) {
            verbose_option = true;
        }
        arg ++;
    }
    if (got_s && got_E && got_b) 
        initalizeCache(s, E, b);
    else printArgError(argv[0]);
    return fp;    
}

int main(int argc, char *argv[])
{
    char buf[MAXLINE];
    FILE *fp = optionsParser(argc, argv);
    while (true) {
        fgets(buf, MAXLINE, fp);
        if (feof(fp)) break;
        eval(buf);
    }
    freeCache();
    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
