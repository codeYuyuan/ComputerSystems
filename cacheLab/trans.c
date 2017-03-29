 /*
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 * Author: Iris Yuan
 * (note on style: in trans.c and csim.c, I comment using lower-case letters for readability.)
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

    /* illegal to have arrays or more than 12 local variables defined */
    /* 6 variables are initialized in this function */

    int i; 
    int j; 
    int row; 
    int column;
    int diagonal = 0;
    int temp = 0;

    /* there are three test cases N x M: 32 x 32, 64 x 64, and 61 x 67 */

    if (N == 32) {
        for (column = 0; column < N; column += 8) { /* iterate across block structure */
            for (row = 0; row < N; row += 8) {
                for (i = row; i < row + 8; i++) { /* iterate through each block */
                    for (j = column; j < column + 8; j++) {
                        if (i != j) {
                            B[j][i] = A[i][j];
                        } else {
                            /* reduce conflict misses M < i*j in B by storing in temp instead of missing in B[j][i] */
                            temp = A[i][j];
                            diagonal = i;
                        }
                    }
                    if (row == column) { /* store diagonal element */
                        B[diagonal][diagonal] = temp; 
                    }
                }   
            }
        }
    } else if (N == 64) {
        for (column = 0; column < N; column += 4) { 
            for (row = 0; row < N; row += 4) {
                for (i = row; i < row + 4; i++) { /* iterate over the rows */
                    for (j = column; j < column + 4; j++) {
                        if (i != j) { /* if not a diagonal element */
                            B[j][i] = A[i][j];
                        } else {
                            temp = A[i][j]; 
                            diagonal = i;
                        }
                    }
                    if (row == column) {
                        B[diagonal][diagonal] = temp;
                    }
                }   
            }
        }
    } 
    else { /* test case 61 x 67 may lead to segmentation fault when iterating over the row; to avoid, must check that j < N */
        for (column = 0; column < M; column += 16) { /* same as above, iterating over columns */
            for (row = 0; row < N; row += 16) {     
                for (i = row; (i < row + 16) && (i < N); i++) {
                    for (j = column; (j < column + 16) && (j < M); j++) {
                        if (i != j) { /* same as above */
                            B[j][i] = A[i][j];
                        } else {
                            temp = A[i][j];
                            diagonal = i;
                        }
                    }
                    if (row == column) {
                        B[diagonal][diagonal] = temp;
                    }
                }
            }
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
    registerTransFunction(transpose_submit, transpose_submit_desc); 

    /* Register any additional transpose functions */

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