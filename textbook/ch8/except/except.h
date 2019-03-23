#ifndef __EXCEPT_H
#define __EXCEPT_H

#include <setjmp.h>

#define T Except_T


#define TRY do {    \
        Except_Frame Except_frame;  \
        Except_frame.prev = Except_stack;   \
        Except_stack = &Except_frame;   \
        int Except_flag = setjmp(Except_frame.env); \
        if (Except_flag == EXCEPT_INITIALIZED) {    \
            Except_flag = EXCEPT_FINALIZED;

#define CATCH(e) if (Except_flag == EXCEPT_RAISED)  \
            Except_flag = EXCEPT_HANDLED;   \
        } else if (Except_flag == EXCEPT_RAISED \
            && Except_frame.exception == &e) {

#define ELSE if (Except_flag == EXCEPT_RAISED) \
            Except_flag = EXCEPT_HANDLED;   \
        } else {

#define FINALLY if (Except_flag == EXCEPT_RAISED)   \
                Except_flag = EXCEPT_HANDLED;   \
        }   \
        if (Except_flag == EXCEPT_HANDLED   \
            || Except_flag == EXCEPT_FINALIZED) {

#define END_TRY if (Except_flag == EXCEPT_RAISED)   \
                Except_flag= EXCEPT_HANDLED;    \
        }   \
        Except_stack = Except_frame.prev;   \
        if (Except_flag == EXCEPT_RAISED)   \
            Except_raise(Except_frame.exception, Except_frame.fileName, \
                Except_frame.lineNum);  \
        }while (0);


#define RAISE(e) Except_raise(&e, __FILE__, __LINE__)
#define RERAISE {   \
                    Except_stack = Except_stack->prev;       \
                    Except_raise(Except_frame.exception,    \
                                Except_frame.fileName,      \
                                Except_frame.lineNum);      \
                }

typedef struct {
    const char *errMsg;
}T;

typedef struct ExceptFrame{
    const char *fileName;
    int lineNum;
    T *exception;
    jmp_buf env;
    struct ExceptFrame *prev;
}Except_Frame;

extern Except_Frame *Except_stack;

enum {
    EXCEPT_INITIALIZED,
    EXCEPT_RAISED,
    EXCEPT_HANDLED,
    EXCEPT_FINALIZED, 
};

extern void Except_raise(T *exception, const char *fileName, int lineNum);

#undef T
#endif