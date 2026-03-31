#include <iostream>
#include <cuda_runtime.h>

int main(){
    int deviceCount;
    cudaGetDeviceCount(&deviceCount);
    std::cout << "Number of CUDA devices: " << deviceCount << std::endl;

    for (int i = 0; i < deviceCount; ++i) {
        cudaDeviceProp deviceProp;
        cudaGetDeviceProperties(&deviceProp, i);
        std::cout << "Device " << i << ": " << deviceProp.name << std::endl;
        std::cout << "  Compute capability: " << deviceProp.major << "." << deviceProp.minor << std::endl;
        std::cout << "  Multi-processor count: " << deviceProp.multiProcessorCount << std::endl;
        std::cout << "  Total global memory: " << deviceProp.totalGlobalMem / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Shared memory per block: " << deviceProp.sharedMemPerBlock / 1024 << " KB" << std::endl;
        std::cout << "  Registers per block: " << deviceProp.regsPerBlock << std::endl;
        std::cout << "  Warp size: " << deviceProp.warpSize << std::endl;
        std::cout << "  Max threads per block: " << deviceProp.maxThreadsPerBlock << std::endl;
        std::cout << "  Max thread dimensions: (" 
                  << deviceProp.maxThreadsDim[0] << ", "
                  << deviceProp.maxThreadsDim[1] << ", "
                  << deviceProp.maxThreadsDim[2] << ")" << std::endl;
        std::cout << "  Max grid size: (" 
                  << deviceProp.maxGridSize[0] << ", "
                  << deviceProp.maxGridSize[1] << ", "
                  << deviceProp.maxGridSize[2] << ")" << std::endl;
    }

    return 0;
}