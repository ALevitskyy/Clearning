#include "matmul.h"

int main(){
    int N = 3; // Size of the matrices
    float* A = (float*)malloc(N * N * sizeof(float));
    float* B = (float*)malloc(N * N * sizeof(float));
    float* C = (float*)malloc(N * N * sizeof(float));

    random_fill(A, N);
    random_fill(B, N);

    matmul(A, B, C, N);

    printf("\nResult of A * B:\n");
    print_matrix(C, N);

    free(A);
    free(B);
    free(C);

    return 0;
}