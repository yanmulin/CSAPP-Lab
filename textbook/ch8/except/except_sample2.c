#include <stdio.h>
#include "except.h"

Except_T e1 = {"Exception 1"};
Except_T e2 = {"Exception 2"};
Except_T e3 = {"Exception 3"};

int main() {
    TRY
        printf("In TRY\n");
        RAISE(e3);
    CATCH(e1)
        printf("In CATCH(e1)\n");
    CATCH(e2)
        printf("In CATCH(e2)\n");
    // ELSE 
    //     printf("In ELSE\n");
    FINALLY 
        printf("In FINALLY\n");
    END_TRY
    return 0;
}