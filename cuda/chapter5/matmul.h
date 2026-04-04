#include <stdio.h>
#include <stdlib.h>

void matmul(const float* A, const float* B, float* C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
    }
}

void random_fill(float* matrix, int N) {
    for (int i = 0; i < N * N; ++i) {
        matrix[i] = (float)rand() / RAND_MAX;
    }
}

void print_matrix(const float* matrix, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            printf("%f ", matrix[i * N + j]);
        }
        printf("\n");
    }
}

void check_matrix_equal(const float* A, const float* B, int N) {
    for (int i = 0; i < N * N; ++i) {
        if (abs(A[i] - B[i]) > 1e-3) {
            printf("Matrices are not equal at index %d: %f vs %f\n", i, A[i], B[i]);
            return;
        }
    }
    printf("Matrices are equal.\n");
}

