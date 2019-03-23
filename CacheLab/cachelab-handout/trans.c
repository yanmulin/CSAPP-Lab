/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"


int is_transpose(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, k, h;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 

    if (M == 32 && N == 32) {
        for (i=0;i<N;i+=8) {
            for (j=0;j<M;j+=8) {
                for (k=0;k<8;k++) {
                    temp_value1 = A[i+k][j+0];
                    temp_value2 = A[i+k][j+1];
                    temp_value3 = A[i+k][j+2];
                    temp_value4 = A[i+k][j+3];
                    temp_value5 = A[i+k][j+4];
                    temp_value6 = A[i+k][j+5];
                    temp_value7 = A[i+k][j+6];
                    temp_value8 = A[i+k][j+7];

                    B[j+0][i+k] = temp_value1;
                    B[j+1][i+k] = temp_value2;
                    B[j+2][i+k] = temp_value3;
                    B[j+3][i+k] = temp_value4;
                    B[j+4][i+k] = temp_value5;
                    B[j+5][i+k] = temp_value6;
                    B[j+6][i+k] = temp_value7;
                    B[j+7][i+k] = temp_value8;

                }            
            }
        }
    } else if (M == 64 && N == 64) {
        for (i=0;i<N;i+=8) {
            for (j=0;j<M;j+=8) {
                for (k=0;k<4;k++) {
                    temp_value1 = A[i+k][j+0];
                    temp_value2 = A[i+k][j+1];
                    temp_value3 = A[i+k][j+2];
                    temp_value4 = A[i+k][j+3];
                    temp_value5 = A[i+k][j+4];
                    temp_value6 = A[i+k][j+5];
                    temp_value7 = A[i+k][j+6];
                    temp_value8 = A[i+k][j+7];

                    B[j+0][i+k] = temp_value1;
                    B[j+1][i+k] = temp_value2;
                    B[j+2][i+k] = temp_value3;
                    B[j+3][i+k] = temp_value4;
                    B[j+0][i+k+4] = temp_value5;
                    B[j+1][i+k+4] = temp_value6;
                    B[j+2][i+k+4] = temp_value7;
                    B[j+3][i+k+4] = temp_value8;
                }

                // Step2: move block2&3
                for (k=0;k<4;k++) {

                    temp_value5 = B[j+k][i+4];
                    temp_value6 = B[j+k][i+5];
                    temp_value7 = B[j+k][i+6];
                    temp_value8 = B[j+k][i+7];

                    temp_value1 = A[i+4][j+k];
                    temp_value2 = A[i+5][j+k];
                    temp_value3 = A[i+6][j+k];
                    temp_value4 = A[i+7][j+k];

                    B[j+k][i+4] = temp_value1;
                    B[j+k][i+5] = temp_value2;
                    B[j+k][i+6] = temp_value3;
                    B[j+k][i+7] = temp_value4;

                    B[j+k+4][i+0] = temp_value5;
                    B[j+k+4][i+1] = temp_value6;
                    B[j+k+4][i+2] = temp_value7;
                    B[j+k+4][i+3] = temp_value8;

                }

                for (k=0;k<4;k++) {
                    temp_value1 = A[i+4][j+k+4]; 
                    temp_value2 = A[i+5][j+k+4]; 
                    temp_value3 = A[i+6][j+k+4]; 
                    temp_value4 = A[i+7][j+k+4]; 

                    B[j+4+k][i+4] = temp_value1; 
                    B[j+4+k][i+5] = temp_value2; 
                    B[j+4+k][i+6] = temp_value3; 
                    B[j+4+k][i+7] = temp_value4; 
                }
            }
        }
    } else if (M == 61 && N == 67) {
        for(i=0;i<N;i+=16){
            for(j=0;j<M;j+=16){
                for(k=i;k<i+16&&k<N;k++){
                    for(h=j;h<j+16&&h<M;h++){
                        B[h][k]=A[k][h];
                    }
                }
            }
        }
    } else { 
       for (i = 0; i < N; i++) {
            for (j = 0; j < M; j++) {
                B[j][i] = A[i][j];
            }
        }    
    }
}

char transpose_simple1_desc[] = "8 rows";
void transpose_simple1(int M, int N, int A[N][M], int B[M][N]) 
{
    for (int i=0;i<N;i+=8) {
        for (int j=0;j<M;j+=8) {
            for (int m=i;m<i+8;m++) {
                for (int n=j;n<j+8;n++) {
                    B[n][m] = A[m][n];
                }
            }
        }
    }
}

char transpose_simple2_desc[] = "8 rows plus";
void transpose_simple2(int M, int N, int A[N][M], int B[M][N]) 
{
    for (int i=0;i<N;i+=8) {
        for (int j=0;j<M;j+=8) {
            for (int k=0;k<8;k++) {
                int temp_value1 = A[i+k][j+0];
                int temp_value2 = A[i+k][j+1];
                int temp_value3 = A[i+k][j+2];
                int temp_value4 = A[i+k][j+3];
                int temp_value5 = A[i+k][j+4];
                int temp_value6 = A[i+k][j+5];
                int temp_value7 = A[i+k][j+6];
                int temp_value8 = A[i+k][j+7];

                B[j+0][i+k] = temp_value1;
                B[j+1][i+k] = temp_value2;
                B[j+2][i+k] = temp_value3;
                B[j+3][i+k] = temp_value4;
                B[j+4][i+k] = temp_value5;
                B[j+5][i+k] = temp_value6;
                B[j+6][i+k] = temp_value7;
                B[j+7][i+k] = temp_value8;

            }            
        }
    }
}

char transpose_simple3_desc[] = "64*64: 4*4block 3 steps";
void transpose_simple3(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    // Step1: move block1&2 in 8*8
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n] = temp_value2;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n] = temp_value3;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n] = temp_value4;
                B[j+3][i+n+4] = temp_value8;
            }
        }
    }

    // Step2: move block2&3
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = B[j+n][i+4];
                temp_value2 = B[j+n][i+5];
                temp_value3 = B[j+n][i+6];
                temp_value4 = B[j+n][i+7];

                B[j+4+n][i+0] = temp_value1;
                B[j+4+n][i+1] = temp_value2;
                B[j+4+n][i+2] = temp_value3;
                B[j+4+n][i+3] = temp_value4;

            }

            for (n=0;n<4;n++) {
                temp_value1 = A[i+4+n][j+0]; 
                temp_value2 = A[i+4+n][j+1]; 
                temp_value3 = A[i+4+n][j+2]; 
                temp_value4 = A[i+4+n][j+3]; 

                B[j+0][i+4+n] = temp_value1; 
                B[j+1][i+4+n] = temp_value2; 
                B[j+2][i+4+n] = temp_value3; 
                B[j+3][i+4+n] = temp_value4; 
            }
        }
    }

    // Step3: move block4
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = A[i+4+n][j+4];
                temp_value2 = A[i+4+n][j+5];
                temp_value3 = A[i+4+n][j+6];
                temp_value4 = A[i+4+n][j+7];

                B[j+4][i+4+n] = temp_value1;
                B[j+5][i+4+n] = temp_value2;
                B[j+6][i+4+n] = temp_value3;
                B[j+7][i+4+n] = temp_value4;

            }
        }
    }

}

char transpose_simple4_desc[] = "64*64: 4*4block 2 steps";
void transpose_simple4(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    // Step1: move block1&2 in 8*8
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+1][i+n] = temp_value2;
                B[j+2][i+n] = temp_value3;
                B[j+3][i+n] = temp_value4;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n+4] = temp_value8;
            }

        }
    }


    // Step2: move block2&3&4
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = B[j+n][i+4];
                temp_value2 = B[j+n][i+5];
                temp_value3 = B[j+n][i+6];
                temp_value4 = B[j+n][i+7];

                B[j+4+n][i+0] = temp_value1;
                B[j+4+n][i+1] = temp_value2;
                B[j+4+n][i+2] = temp_value3;
                B[j+4+n][i+3] = temp_value4;

                temp_value1 = A[i+4+n][j+0]; 
                temp_value2 = A[i+4+n][j+1]; 
                temp_value3 = A[i+4+n][j+2]; 
                temp_value4 = A[i+4+n][j+3]; 
                temp_value5 = A[i+4+n][j+4];
                temp_value6 = A[i+4+n][j+5];
                temp_value7 = A[i+4+n][j+6];
                temp_value8 = A[i+4+n][j+7];

                B[j+0][i+4+n] = temp_value1; 
                B[j+1][i+4+n] = temp_value2; 
                B[j+2][i+4+n] = temp_value3; 
                B[j+3][i+4+n] = temp_value4; 
                B[j+4][i+4+n] = temp_value5;
                B[j+5][i+4+n] = temp_value6;
                B[j+6][i+4+n] = temp_value7;
                B[j+7][i+4+n] = temp_value8;
            }
        }
    }
}
char transpose_simple5_desc[] = "64*64: 4*4block 3 steps plus";
void transpose_simple5(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 

    // Step1: move block1&2 in 8*8
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+1][i+n] = temp_value2;
                B[j+2][i+n] = temp_value3;
                B[j+3][i+n] = temp_value4;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n+4] = temp_value8;
            }

            // Step2: move block2&3
            for (n=0;n<4;n++) {
                temp_value1 = B[j+0][i+n+4];
                temp_value2 = B[j+1][i+n+4];
                temp_value3 = B[j+2][i+n+4];
                temp_value4 = B[j+3][i+n+4];

                temp_value5 = A[i+4][j+n];
                temp_value6 = A[i+5][j+n];
                temp_value7 = A[i+6][j+n];
                temp_value8 = A[i+7][j+n];

                B[j+4][i+n] = temp_value1;
                B[j+5][i+n] = temp_value2;
                B[j+6][i+n] = temp_value3;
                B[j+7][i+n] = temp_value4;


                B[j+0][i+4+n] = temp_value5; 
                B[j+1][i+4+n] = temp_value6; 
                B[j+2][i+4+n] = temp_value7; 
                B[j+3][i+4+n] = temp_value8; 
            }

            // Step3: move block4
            for (n=0;n<4;n++) {
                temp_value1 = A[i+4+n][j+4];
                temp_value2 = A[i+4+n][j+5];
                temp_value3 = A[i+4+n][j+6];
                temp_value4 = A[i+4+n][j+7];

                B[j+4][i+4+n] = temp_value1;
                B[j+5][i+4+n] = temp_value2;
                B[j+6][i+4+n] = temp_value3;
                B[j+7][i+4+n] = temp_value4;

            }
        }
    }

}

char transpose_simple6_desc[] = "4 rows plus";
void transpose_simple6(int M, int N, int A[N][M], int B[M][N]) 
{
    int temp_value1, temp_value2, temp_value3, temp_value4;
    for (int i=0;i<N;i+=4) {
        for (int j=0;j<M;j+=4) {
            for (int m=0;m<4;m++) {
                temp_value1 = A[i+m][j+0];
                temp_value2 = A[i+m][j+1];
                temp_value3 = A[i+m][j+2];
                temp_value4 = A[i+m][j+3];

                B[j+0][i+m] = temp_value1;
                B[j+1][i+m] = temp_value2;
                B[j+2][i+m] = temp_value3;
                B[j+3][i+m] = temp_value4;

            }
        }
    }
}

char transpose_simple7_desc[] = "64*64: 4*4block 3 steps plus";
void transpose_simple7(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            // Step1: move block1&2 in 8*8
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+1][i+n] = temp_value2;
                B[j+2][i+n] = temp_value3;
                B[j+3][i+n] = temp_value4;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n+4] = temp_value8;
            }

            // Step2: move block2&3
            for (n=0;n<4;n++) {
                temp_value1 = B[j+n][i+4];
                temp_value2 = B[j+n][i+5];
                temp_value3 = B[j+n][i+6];
                temp_value4 = B[j+n][i+7];

                temp_value5 = A[i+4][j+n+4];
                temp_value6 = A[i+5][j+n+4];
                temp_value7 = A[i+6][j+n+4];
                temp_value8 = A[i+7][j+n+4];

                B[j+n+4][i+0] = temp_value1;
                B[j+n+4][i+1] = temp_value2;
                B[j+n+4][i+2] = temp_value3;
                B[j+n+4][i+3] = temp_value4;
                B[j+n+4][i+4] = temp_value5;
                B[j+n+4][i+5] = temp_value6;
                B[j+n+4][i+6] = temp_value7;
                B[j+n+4][i+7] = temp_value8;

            }

            for (n=0;n<4;n++) {
                temp_value1 = A[i+4+n][j+0]; 
                temp_value2 = A[i+4+n][j+1]; 
                temp_value3 = A[i+4+n][j+2]; 
                temp_value4 = A[i+4+n][j+3]; 

                B[j+0][i+4+n] = temp_value1; 
                B[j+1][i+4+n] = temp_value2; 
                B[j+2][i+4+n] = temp_value3; 
                B[j+3][i+4+n] = temp_value4; 
            }

        }
    }
}

char transpose_zhihu_desc[] = "64*64: zhihu";
void transpose_zhihu(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            // Step1: move block1&2 in 8*8
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+1][i+n] = temp_value2;
                B[j+2][i+n] = temp_value3;
                B[j+3][i+n] = temp_value4;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n+4] = temp_value8;
            }

            // Step2: move block2&3
            for (n=0;n<4;n++) {

                temp_value5 = B[j+n][i+4];
                temp_value6 = B[j+n][i+5];
                temp_value7 = B[j+n][i+6];
                temp_value8 = B[j+n][i+7];

                temp_value1 = A[i+4][j+n];
                temp_value2 = A[i+5][j+n];
                temp_value3 = A[i+6][j+n];
                temp_value4 = A[i+7][j+n];

                B[j+n][i+4] = temp_value1;
                B[j+n][i+5] = temp_value2;
                B[j+n][i+6] = temp_value3;
                B[j+n][i+7] = temp_value4;

                B[j+n+4][i+0] = temp_value5;
                B[j+n+4][i+1] = temp_value6;
                B[j+n+4][i+2] = temp_value7;
                B[j+n+4][i+3] = temp_value8;

            }

            for (n=0;n<4;n++) {
                temp_value1 = A[i+4][j+n+4]; 
                temp_value2 = A[i+5][j+n+4]; 
                temp_value3 = A[i+6][j+n+4]; 
                temp_value4 = A[i+7][j+n+4]; 

                B[j+4+n][i+4] = temp_value1; 
                B[j+4+n][i+5] = temp_value2; 
                B[j+4+n][i+6] = temp_value3; 
                B[j+4+n][i+7] = temp_value4; 
            }

        }
    }
}

char transpose_simple9_desc[] = "64*64: 4*4block 3 steps plus";
void transpose_simple9(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            // Step1: move block1&2 in 8*8
            for (n=0;n<4;n++) {
                temp_value1 = A[i+n][j+0];
                temp_value2 = A[i+n][j+1];
                temp_value3 = A[i+n][j+2];
                temp_value4 = A[i+n][j+3];
                temp_value5 = A[i+n][j+4];
                temp_value6 = A[i+n][j+5];
                temp_value7 = A[i+n][j+6];
                temp_value8 = A[i+n][j+7];

                B[j+0][i+n] = temp_value1;
                B[j+1][i+n] = temp_value2;
                B[j+2][i+n] = temp_value3;
                B[j+3][i+n] = temp_value4;
                B[j+0][i+n+4] = temp_value5;
                B[j+1][i+n+4] = temp_value6;
                B[j+2][i+n+4] = temp_value7;
                B[j+3][i+n+4] = temp_value8;
            }

            // Step2: move block2&3
            for (n=0;n<4;n++) {

                temp_value5 = B[j+n][i+4];
                temp_value6 = B[j+n][i+5];
                temp_value7 = B[j+n][i+6];
                temp_value8 = B[j+n][i+7];

                temp_value1 = A[i+4][j+n];
                temp_value2 = A[i+5][j+n];
                temp_value3 = A[i+6][j+n];
                temp_value4 = A[i+7][j+n];

                B[j+n][i+4] = temp_value1;
                B[j+n][i+5] = temp_value2;
                B[j+n][i+6] = temp_value3;
                B[j+n][i+7] = temp_value4;

                B[j+n+4][i+0] = temp_value5;
                B[j+n+4][i+1] = temp_value6;
                B[j+n+4][i+2] = temp_value7;
                B[j+n+4][i+3] = temp_value8;

            }

            for (n=0;n<4;n++) {
                temp_value1 = A[i+n+4][j+4]; 
                temp_value2 = A[i+n+4][j+5]; 
                temp_value3 = A[i+n+4][j+6]; 
                temp_value4 = A[i+n+4][j+7]; 

                B[j+4][i+n+4] = temp_value1; 
                B[j+5][i+n+4] = temp_value2; 
                B[j+6][i+n+4] = temp_value3; 
                B[j+7][i+n+4] = temp_value4; 
            }

        }
    }
}

char transpose_simple11_desc[] = "61*67: 16*16";
void transpose_simple11(int M, int N, int A[N][M], int B[M][N]) {  
    int i, j, k, h;
    for(i=0;i<N;i+=16){
        for(j=0;j<M;j+=16){
            for(k=i;k<i+16&&k<N;k++){
                for(h=j;h<j+16&&h<M;h++){
                    B[h][k]=A[k][h];
                }
            }
        }
    }
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
    /* Register your solution function */
    // registerTransFunction(transpose_simple1, transpose_simple1_desc);
    // registerTransFunction(transpose_simple2, transpose_simple2_desc);
    // registerTransFunction(transpose_simple3, transpose_simple3_desc);
    // registerTransFunction(transpose_simple4, transpose_simple4_desc);
    // registerTransFunction(transpose_simple5, transpose_simple5_desc); // 1643  misses
    // registerTransFunction(transpose_simple6, transpose_simple6_desc); // 1667 misses
    // registerTransFunction(transpose_simple7, transpose_simple7_desc); // 1472 misses
    // registerTransFunction(transpose_simple9, transpose_simple9_desc); // misses
    // registerTransFunction(transpose_simple11, transpose_simple11_desc); // misses
    // registerTransFunction(transpose_zhihu, transpose_zhihu_desc); // misses
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */
    // registerTransFunction(trans, trans_desc); 

}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

