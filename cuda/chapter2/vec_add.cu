#include <stdio.h>
#include <stdlib.h>

__global__
void vecAddKernel(float* A, float* B, float* C, int n){
    int i = threadIdx.x + blockDim.x * blockIdx.x;
    if (i <n){
        C[i] = A[i] + B[i];
    }
}

void vecAdd(float* A, float* B, float* C, int n){
    float *A_d, *B_d, *C_d;
    int size = n * sizeof(float);

    cudaMalloc((void**)&A_d, size);
    cudaMalloc((void**)&B_d, size);
    cudaMalloc((void**)&C_d, size);

    cudaMemcpy(A_d, A, size, cudaMemcpyHostToDevice);
    cudaMemcpy(B_d, B, size, cudaMemcpyHostToDevice);

    vecAddKernel<<<ceil(n/256.0), 256>>>(A_d, B_d, C_d, n);

    cudaMemcpy(C, C_d, size, cudaMemcpyDeviceToHost);

    cudaFree(A_d);
    cudaFree(B_d);
    cudaFree(C_d);
}

int main(){
    int n = 1<<20;
    float *A_h, *B_h, *C_h;
    int size = n*sizeof(float);
    A_h = (float*)malloc(size);
    B_h = (float*)malloc(size);
    C_h = (float*)malloc(size);

    for (int i=0; i< n; ++i){
        A_h[i] = i;
        B_h[i] = i*2;
    }

    vecAdd(A_h, B_h, C_h, n);

    printf("C[0] = %f\n", C_h[0]);
    printf("C[n-1] = %f\n", C_h[n-1]);

    free(A_h);
    free(B_h);
    free(C_h);

    return 0;
}