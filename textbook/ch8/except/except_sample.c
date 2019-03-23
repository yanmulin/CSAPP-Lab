#include <stdio.h>
#include "except.h"

Except_T e = {"Nothing happened."};
Except_T another_e = {"Something happened."};

int main() {
    // TRY
    do {
        // Initialization
        Except_Frame Except_frame;
        Except_frame.prev = Except_stack;
        Except_stack = &Except_frame;
        // Save longjmp environment
        int Except_flag = setjmp(Except_frame.env);
        if (Except_flag == EXCEPT_INITIALIZED) {
            // if EXCEPT_FINALIZED is a useless status?
            Except_flag = EXCEPT_FINALIZED;

        // ...    
        RAISE(another_e);

        // CATCH(e)
            if (Except_flag == EXCEPT_RAISED) 
                Except_flag = EXCEPT_HANDLED;
        } else if (Except_flag == EXCEPT_RAISED && Except_frame.exception == &e) {

        // ...    
        printf("Test e happened\n");

        // ELSE
            if (Except_flag == EXCEPT_RAISED) 
                Except_flag = EXCEPT_HANDLED;
        } else {

        // ...
        printf("Test another_e happened\n");
        RERAISE;

        // FINALLY
            if (Except_flag == EXCEPT_RAISED)
                Except_flag = EXCEPT_HANDLED;
        }
        if (Except_flag == EXCEPT_HANDLED 
            || Except_flag == EXCEPT_FINALIZED) {

                // ...
                printf("Test finally\n");
        
        // END_TRY
            if (Except_flag == EXCEPT_RAISED)
                Except_flag= EXCEPT_HANDLED;
        }
        Except_stack = Except_frame.prev;
        if (Except_flag == EXCEPT_RAISED)
            Except_raise(Except_frame.exception, Except_frame.fileName,
                Except_frame.lineNum);
    } while (0);
}

