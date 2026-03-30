#define STB_IMAGE_WRITE_IMPLEMENTATION  // Only in ONE .c/.cu file
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHANNELS 3 
#define BLUR_SIZE 2

#define cudaCheckError() { \
    cudaError_t e=cudaGetLastError(); \
    if(e!=cudaSuccess) { \
        printf("CUDA Error %s:%d: %s\n",__FILE__,__LINE__,cudaGetErrorString(e)); \
        exit(EXIT_FAILURE); \
    } \
}


__global__
void blurKernel(
    unsigned char * out, unsigned char * in, int w, int h
){
    int col = blockIdx.x*blockDim.x+threadIdx.x;
    int row = blockIdx.y*blockDim.y+threadIdx.y;
    if (col < w && row < h){
        int pixValR = 0;
        int pixValG = 0;
        int pixValB = 0;
        int pixels = 0;
        
        for (int blurRow=-BLUR_SIZE; blurRow<BLUR_SIZE+1; ++blurRow){
            for(int blurCol=-BLUR_SIZE; blurCol<BLUR_SIZE+1; ++blurCol){
                int curRow = row+blurRow;
                int curCol = col+blurCol;

                if(curRow>=0 && curRow<h && curCol>=0 && curCol<w){
                    int curOffset = (curRow*w+curCol)*CHANNELS;
                    pixValR += in[curOffset];
                    pixValG += in[curOffset+1];
                    pixValB += in[curOffset+2];
                    ++pixels;
                }
            }
        }
    int grayOffset = row*w+col;
    int RGBOffset = grayOffset*CHANNELS;
    out[RGBOffset] = pixValR/pixels;
    out[RGBOffset+1] = pixValG/pixels;
    out[RGBOffset+2] = pixValB/pixels;
        

    }
    
}

// Generate random RGB noise data
void generate_random_data(unsigned char* data, int width, int height) {
    srand(time(NULL));
    for (int i = 0; i < width * height * CHANNELS; i++) {
        data[i] = rand() % 256;
    }
}

int main(){
    int width = 1024;
    int height = 768;
    
    // Host memory
    unsigned char *Pin_host = (unsigned char*)malloc(width*height*CHANNELS*sizeof(unsigned char));
    unsigned char *Pout_host = (unsigned char*)malloc(width*height*CHANNELS*sizeof(unsigned char));
    
    // Device memory
    unsigned char *Pin_device, *Pout_device;
    cudaMalloc(&Pin_device, width*height*CHANNELS*sizeof(unsigned char));
    cudaMalloc(&Pout_device, width*height*CHANNELS*sizeof(unsigned char));
    
    // Generate random colored noise on host
    generate_random_data(Pin_host, width, height);
    
    // Write original color image as PNG
    stbi_write_png("noise_color.png", width, height, CHANNELS, Pin_host, width * CHANNELS);
    
    // Copy input data to device
    cudaMemcpy(Pin_device, Pin_host, width*height*CHANNELS*sizeof(unsigned char), cudaMemcpyHostToDevice);
    
    // Convert to grayscale using CUDA
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, 
                  (height + blockSize.y - 1) / blockSize.y);
    blurKernel<<<gridSize, blockSize>>>(Pout_device, Pin_device, width, height);
    cudaCheckError();
    
    
    // Copy result back to host
    cudaMemcpy(Pout_host, Pout_device, width*height*CHANNELS*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    cudaCheckError();
    
    // Write grayscale image as PNG
    stbi_write_png("noise_blurred.png", width, height, CHANNELS, Pout_host, width * CHANNELS);
    
    // Cleanup
    cudaFree(Pin_device);
    cudaFree(Pout_device);
    free(Pin_host);
    free(Pout_host);
    
    return 0;
}