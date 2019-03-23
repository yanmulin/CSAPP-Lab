#include <stdio.h>
#include <stdlib.h>
#include "except.h"

#define T Except_T

Except_Frame *Except_stack = NULL;

void Except_raise(T *exception, const char *fileName, int lineNum){
    Except_Frame *p = Except_stack;
    if (p == NULL) {
        fprintf(stderr, "Exception caught at %s:%d: %s\n", 
            fileName, lineNum, exception->errMsg);
        exit(0);
    } else {
        p->exception = exception;
        p->fileName = fileName;
        p->lineNum = lineNum;
        longjmp(p->env, EXCEPT_RAISED);
    }  
}


#undef T