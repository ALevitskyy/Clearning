#include <stdio.h>
#include <stdlib.h>

void vecAdd(float* A_h, float* B_h, float* C_h, int n){
    for (int i=0; i< n; ++i){
        C_h[i] = A_h[i] + B_h[i];
    }
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