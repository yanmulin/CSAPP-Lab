#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
#include <assert.h>

#include "bits.h"

void mytest_isAsciiDigit();
void mytest_isTmax();
void mytest_conditional();
void mytest_isLessOrEqual();
void mytest_floatScale2();
void mytest_floatFloat2Int();

int main() {
    mytest_isAsciiDigit();
    // mytest_isTmax();
    mytest_conditional();    
    mytest_isLessOrEqual();
    // mytest_floatScale2();
    mytest_floatFloat2Int();
    return 0;
}

void mytest_isAsciiDigit() {
    int fail_cnt = 0;
    printf("Test isAsciiDigit()\n");
    for (int i = 0;i<-1;i++){
        int ret = isAsciiDigit(i);
        bool inRange = i >= 30 && i <= 0x39;
        if ((inRange && ret == 0) || (!inRange && ret == 1)) {
            printf("Test %d failed.\n", i);
            fail_cnt ++;
        }
    }
    if (fail_cnt == 0) 
        printf("All passed.\n");
}

void mytest_isTmax() {
    int fail_cnt = 0;
    printf("Test isTmax()\n");
    for (int i=INT_MIN;i<=INT_MAX;i++){
        int ret = isTmax(i);
        bool inRange = (i != INT_MAX);
        if ((inRange && ret == 1) || (!inRange && ret == 0)) {
                printf("Test i=%d failed.\n", i);
                fail_cnt ++;
        }
        if (i == INT_MAX) break;
    }
    if (fail_cnt == 0) 
        printf("All passed.\n");
}

void mytest_conditional() {
    printf("Test conditional()\n");
    int val1 = 0xAAAAAAAA;
    int val2 = 0x55555555;
    assert (conditional(INT_MIN, val1, val2) == val1);
    assert (conditional(-1, val1, val2) == val1);
    assert (conditional(0, val1, val2) == val2);
    assert (conditional(1, val1, val2) == val1);
    assert (conditional(INT_MAX, val1, val2) == val1);
    printf("All passed.\n");
}

void mytest_isLessOrEqual() {
    printf("Test isLessOrEqual()\n");
    assert (isLessOrEqual(-1, 0) == 1);
    assert (isLessOrEqual(-1, 1) == 1);
    assert (isLessOrEqual(-1, -1) == 1);
    assert (isLessOrEqual(0, -1) == 0);
    assert (isLessOrEqual(0, 0) == 1);
    assert (isLessOrEqual(0, 1) == 1);
    assert (isLessOrEqual(1, -1) == 0);
    assert (isLessOrEqual(1, 0) == 0);
    assert (isLessOrEqual(1, 1) == 1);
    printf("All passed.\n");
}

void mytest_floatScale2() {
    printf("Test floatScale2()\n");
    printf("0x%x\n", floatScale2(0x00D55555));
    printf("0x%x\n", floatScale2(0x3f800000));
    // printf("All passed.\n");
}

float Unsigned2Float(unsigned u) {
    return *(float*)(&u);
}

void mytest_floatFloat2Int() {
    printf("Test floatFloat2Int()\n");
    printf("%f, %d\n", Unsigned2Float(0x42D55555), floatFloat2Int(0x42D55555));
    printf("%f, %d\n", Unsigned2Float(0x42D55555), floatFloat2Int(0x42D55555));
    printf("%f, %d\n", Unsigned2Float(0x42D55555), floatFloat2Int(0x42D55555));
    printf("%f, %d\n", Unsigned2Float(0x42D55555), floatFloat2Int(0x42D55555));
}
