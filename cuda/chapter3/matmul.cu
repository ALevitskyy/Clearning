#include <stdio.h>

__global__ void MatrixMulKernel(float* M, float* N, float* P, int Width){
    int row = blockIdx.y*blockDim.y+threadIdx.y;
    int col = blockIdx.x*blockDim.x+threadIdx.x;
    if ((row<Width) && (col<Width)){
        float Pvalue = 0;
        for (int k=0; k<Width; ++k){
            Pvalue += M[row*Width+k]*N[k*Width+col];
        }
        P[row*Width+col] = Pvalue;
    }
}

int main(){
    float matrix1[] = {1,2,3,4,5,6,7,8,9};
    float matrix2[] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    float result[9];
    float *d_matrix1, *d_matrix2, *d_result;
    int size = 9*sizeof(float);
    cudaMalloc(&d_matrix1, size);
    cudaMalloc(&d_matrix2, size);
    cudaMalloc(&d_result, size);
    cudaMemcpy(d_matrix1, matrix1, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_matrix2, matrix2, size, cudaMemcpyHostToDevice);
    dim3 threadsPerBlock(3, 3);
    dim3 blocksPerGrid(1, 1);
    MatrixMulKernel<<<blocksPerGrid, threadsPerBlock>>>(d_matrix1, d_matrix2, d_result, 3);
    cudaMemcpy(result, d_result, size, cudaMemcpyDeviceToHost);
    for (int row=0; row<3; ++row){
        for (int col=0; col<3; ++col){
            int i = row*3+col;
            printf("%f ", result[i]);
        }
        printf("\n");
    }
}

// 1 2 3  9 8 7
// 4 5 6  6 5 4
// 7 8 9  3 2 1