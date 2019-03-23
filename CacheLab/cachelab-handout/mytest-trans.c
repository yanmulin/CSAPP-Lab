#include <stdio.h>
#include <assert.h>


#define MATRIXSIZE 64

void print_matrix(int M, int N, int A[M][N], int block_x, int block_y) {
    int row_offset = (block_y - 1) * 8;
    int col_offset = (block_x - 1) * 8;

    for (int i=0;i<8;i++) {
        for (int j=0;j<8;j++) {
            printf("%2d ", A[i+row_offset][j+col_offset]);
        }
        putchar('\n');
    }

    putchar('\n');
}

void is_transpose(int M, int N, int A[N][M], int B[M][N]) {
    for (int i=0;i<N;i++) {
        for (int j=0;j<M;j++) {
            if (A[i][j] != B[j][i]) {
                printf("i=%d, j=%d break, A[i][j].\n", i, j);
                print_matrix(M, N, A, i % 8 + 1, j / 8 + 1);
                print_matrix(M, N, B, i / 8 + 1, i % 8 + 1);
                return;
            }
        }
    }
}

void transpose_simple3_test_step1(int M, int N, int A[N][M], int B[M][N]) {
    for (int i=0;i<N;i+=8) {
        for (int j=0;j<M;j+=8) {
            for (int n=0;n<4;n++) {
                for (int m=0;m<4;m++) {
                    assert(A[i+n][j+0+m] == B[j+m][i+n]);
                    assert(A[i+n][j+4+m] == B[j+m][i+n+4]);
                }
            }
        }
    }
}

void transpose_simple3_test_step2(int M, int N, int A[N][M], int B[M][N]) {
    for (int i=0;i<N;i+=8) {
        for (int j=0;j<M;j+=8) {
            for (int n=0;n<4;n++) {
                for (int m=0;m<4;m++) {
                    assert (A[i+n][j+4+m] == B[j+4+m][i+n]);
                    assert (A[i+4+n][j+m] == B[j+m][i+4+n]);
                }
            }
        }
    }
}

char transpose_simple3_desc[] = "64*64: 4*4block 3 steps";
void transpose_simple3(int M, int N, int A[N][M], int B[M][N]) 
{
    assert (N == 64 && M == 64);
    int i, j, n, m;
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

    transpose_simple3_test_step1(M, N, A, B);

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

    transpose_simple3_test_step2(M, N, A, B);

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

    // print_matrix(M, N, B, 1, 1);
    is_transpose(M, N, A, B);

    printf("transpose_simple3 done.\n");

}

char transpose_simple4_desc[] = "64*64: 4*4block 2 steps";
void transpose_simple4(int M, int N, int A[N][M], int B[M][N]) 
{
    int i, j, n, m;
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

    transpose_simple3_test_step1(M, N, A, B);

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
    
    print_matrix(M, N, B, 1, 1);

    is_transpose(M, N, A, B);

    printf("transpose_simple4 done.\n");
}

char transpose_simple7_desc[] = "64*64: 4*4block 3 steps plus";
void transpose_simple7(int M, int N, int A[N][M], int B[M][N]) 
{
    assert (N == 64 && M == 64);
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
    is_transpose(M, N, A, B);
}


char transpose_simple8_desc[] = "64*64: 4*4block 3 steps plus";
void transpose_simple8(int M, int N, int A[N][M], int B[M][N]) 
{
    assert (N == 64 && M == 64);
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
                temp_value1 = A[i+4][j+n];
                temp_value2 = A[i+5][j+n];
                temp_value3 = A[i+6][j+n];
                temp_value4 = A[i+7][j+n];

                temp_value5 = B[j+n][i+4];
                temp_value6 = B[j+n][i+5];
                temp_value7 = B[j+n][i+6];
                temp_value8 = B[j+n][i+7];


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

    is_transpose(M, N, A, B);
}

char transpose_simple10_desc[] = "61*67: 16*16";
void transpose_simple10(int M, int N, int A[N][M], int B[M][N]) {
    int i, j, k;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8;
    
    for (i=0;i<N;i+=16) {
        for (j=0;j<M;j+=16) {
            for (k=0;k<16;k++) {
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

                temp_value1 = A[i+k][j+8];                
                temp_value2 = A[i+k][j+9];                
                temp_value3 = A[i+k][j+10];                
                temp_value4 = A[i+k][j+11];                
                temp_value5 = A[i+k][j+12];                
                temp_value6 = A[i+k][j+13];                
                temp_value7 = A[i+k][j+14];                
                temp_value8 = A[i+k][j+15];                

                B[j+8][i+k] = temp_value1;
                B[j+9][i+k] = temp_value2;
                B[j+10][i+k] = temp_value3;
                B[j+11][i+k] = temp_value4;
                B[j+12][i+k] = temp_value5;
                B[j+13][i+k] = temp_value6;
                B[j+14][i+k] = temp_value7;
                B[j+15][i+k] = temp_value8;
            }
        }
    } 
    is_transpose(M, N, A, B);
}

char transpose_zhihu_desc[] = "64*64: zhihu";
void transpose_zhihu(int M, int N, int A[N][M], int B[M][N]) 
{
    assert (N == 64 && M == 64);
    int i, j, k;
    int temp_value1, temp_value2, temp_value3, temp_value4, 
    temp_value5, temp_value6, temp_value7, temp_value8; 
    
    for (i=0;i<N;i+=8) {
        for (j=0;j<M;j+=8) {
            // Step1: move block1&2 in 8*8
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
    is_transpose(M, N, A, B);
}

void generate_matrix(int M, int N, int A[N][M]) {
    int num = 1;
    for (int i=0;i<N;i++) {
        for (int j=0;j<M;j++) {
            A[i][j] = i;
        }
    }
}



int main() {
    int A[MATRIXSIZE][MATRIXSIZE], B[MATRIXSIZE][MATRIXSIZE];
    generate_matrix(MATRIXSIZE, MATRIXSIZE, A);

    print_matrix(MATRIXSIZE, MATRIXSIZE, A, 1, 2);

    // transpose_simple3(MATRIXSIZE, MATRIXSIZE, A, B);
    // transpose_simple4(MATRIXSIZE, MATRIXSIZE, A, B);
    // transpose_simple7(MATRIXSIZE, MATRIXSIZE, A, B);
    // transpose_simple8(MATRIXSIZE, MATRIXSIZE, A, B);
    // transpose_simple10(MATRIXSIZE, MATRIXSIZE, A, B);
    transpose_zhihu(MATRIXSIZE, MATRIXSIZE, A, B);

    return 0;
}