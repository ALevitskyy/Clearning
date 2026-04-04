#define TILE_WIDTH 16

#include <stdio.h>
#include "matmul.h"

__global__ void matrixMulKernel(float* M, float* N, float* P, int Width){
    __shared__ float Mds[TILE_WIDTH][TILE_WIDTH];
    __shared__ float Nds[TILE_WIDTH][TILE_WIDTH];

    int bx = blockIdx.x; int by = blockIdx.y;
    int tx = threadIdx.x; int ty = threadIdx.y;

    int Row = by * TILE_WIDTH +ty;
    int Col = bx * TILE_WIDTH + tx;

    float Pvalue = 0;
    for (int ph = 0; ph < ceil(Width / (float)TILE_WIDTH); ++ph){
        if ((Row<Width)&&(ph*TILE_WIDTH+tx)<Width)
            Mds[ty][tx] = M[Row*Width + ph*TILE_WIDTH + tx];
        else Mds[ty][tx] = 0.0f;
        if ((ph*TILE_WIDTH+ty) < Width && Col<Width)
            Nds[ty][tx] = N[(ph*TILE_WIDTH + ty)*Width+Col];
        else Nds[ty][tx] = 0.0f;
        __syncthreads();

        for (int k = 0; k < TILE_WIDTH; ++k){
            Pvalue += Mds[ty][k] * Nds[k][tx];
        };
        __syncthreads();

    }

    if (Row < Width && Col < Width)
        P[Row*Width+Col] = Pvalue;
}

size_t calculate_appropriate_SM_usage(int Width){
    cudaDeviceProp deviceProp;
    cudaGetDeviceProperties(&deviceProp, 0);
    int sharedMemPerBlock = deviceProp.sharedMemPerBlock;
    int maxThreadsPerBlock = deviceProp.maxThreadsPerBlock;
    int threadsPerBlock = TILE_WIDTH * TILE_WIDTH;
    int sharedMemUsage = 2 * TILE_WIDTH * TILE_WIDTH * sizeof(float); //
    if (threadsPerBlock > maxThreadsPerBlock) {
        printf("Warning: Threads per block exceed device limit. Adjusting to %d threads per block.\n", maxThreadsPerBlock);
        threadsPerBlock = maxThreadsPerBlock;
    }
    if (sharedMemUsage > sharedMemPerBlock) {
        printf("Warning: Shared memory usage per block exceeds device limit. Adjusting TILE_WIDTH to fit within shared memory.\n");
        int maxTileWidth = sqrt(sharedMemPerBlock / (2 * sizeof(float)));
        printf("Adjusting TILE_WIDTH from %d to %d.\n", TILE_WIDTH, maxTileWidth);
        return maxTileWidth;
    }
    return TILE_WIDTH;
}

int main(){
    int Width = 1022;
    size_t size = Width * Width * sizeof(float);

    float* h_M = (float*)malloc(size);
    float* h_N = (float*)malloc(size);
    float* h_P = (float*)malloc(size);
    float* h_Pd = (float*)malloc(size);

    random_fill(h_M, Width);
    random_fill(h_N, Width);

    matmul(h_M, h_N, h_P, Width);

    float* d_M; float* d_N; float* d_P;
    cudaMalloc(&d_M, size);
    cudaMalloc(&d_N, size);
    cudaMalloc(&d_P, size);

    cudaMemcpy(d_M, h_M, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_N, h_N, size, cudaMemcpyHostToDevice);

    dim3 dimBlock(TILE_WIDTH, TILE_WIDTH);
    dim3 dimGrid((Width + TILE_WIDTH - 1) / TILE_WIDTH, (Width + TILE_WIDTH - 1) / TILE_WIDTH);
    matrixMulKernel<<<dimGrid, dimBlock>>>(d_M, d_N, d_P, Width);

    cudaMemcpy(h_Pd, d_P, size, cudaMemcpyDeviceToHost);

    check_matrix_equal(h_P, h_Pd, Width);


    free(h_M); free(h_N); free(h_P); free(h_Pd);
    cudaFree(d_M); cudaFree(d_N); cudaFree(d_P);
}

