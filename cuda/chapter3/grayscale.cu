#define STB_IMAGE_WRITE_IMPLEMENTATION  // Only in ONE .c/.cu file
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CHANNELS 3 

#define cudaCheckError() { \
    cudaError_t e=cudaGetLastError(); \
    if(e!=cudaSuccess) { \
        printf("CUDA Error %s:%d: %s\n",__FILE__,__LINE__,cudaGetErrorString(e)); \
        exit(EXIT_FAILURE); \
    } \
}


__global__
void colortoGrayscaleConvertion(
    unsigned char * Pout, unsigned char * Pin, int width, int height
){
    int col = blockIdx.x*blockDim.x+threadIdx.x;
    int row = blockIdx.y*blockDim.y+threadIdx.y;
    if(col<width && row<height){
        int grayOffset = row*width+col;
        int rgbOffset = grayOffset*CHANNELS;
        unsigned char r = Pin[rgbOffset];
        unsigned char g = Pin[rgbOffset+1];
        unsigned char b = Pin[rgbOffset+2];
        unsigned char gray = (unsigned char)(0.21f*r + 0.71f*g + 0.07f*b);
        Pout[grayOffset*CHANNELS] = gray;
        Pout[grayOffset*CHANNELS+1] = gray;
        Pout[grayOffset*CHANNELS+2] = gray;
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
    colortoGrayscaleConvertion<<<gridSize, blockSize>>>(Pout_device, Pin_device, width, height);
    cudaCheckError();
    
    
    // Copy result back to host
    cudaMemcpy(Pout_host, Pout_device, width*height*CHANNELS*sizeof(unsigned char), cudaMemcpyDeviceToHost);
    cudaCheckError();
    
    // Write grayscale image as PNG
    stbi_write_png("noise_grayscale.png", width, height, CHANNELS, Pout_host, width * CHANNELS);
    
    // Cleanup
    cudaFree(Pin_device);
    cudaFree(Pout_device);
    free(Pin_host);
    free(Pout_host);
    
    return 0;
}