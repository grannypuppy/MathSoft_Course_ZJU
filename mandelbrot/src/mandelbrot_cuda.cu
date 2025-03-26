#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <stdio.h>
#include <vector>

// CUDA核函数，计算Mandelbrot集
__global__ void mandelbrotKernel(int* result, double xMin, double yMin, 
                               double xStep, double yStep, 
                               int width, int height, int maxIterations) {
    // 计算当前线程处理的坐标
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    
    if (x < width && y < height) {
        double real = xMin + x * xStep;
        double imag = yMin + y * yStep;
        
        // Mandelbrot迭代
        double zReal = 0;
        double zImag = 0;
        int iterations = 0;
        
        while (zReal * zReal + zImag * zImag <= 4.0 && iterations < maxIterations) {
            double tmp = zReal * zReal - zImag * zImag + real;
            zImag = 2.0 * zReal * zImag + imag;
            zReal = tmp;
            iterations++;
        }
        
        // 存储结果
        result[y * width + x] = iterations;
    }
}

// C++调用包装函数
extern "C" void computeMandelbrotCUDA(int* result, double xMin, double yMin, double xMax, double yMax,
                                    int width, int height, int maxIterations) {
    
    // 分配设备内存
    int* d_result;
    size_t size = width * height * sizeof(int);
    cudaMalloc((void**)&d_result, size);
    
    // 计算步长
    double xStep = (xMax - xMin) / width;
    double yStep = (yMax - yMin) / height;
    
    // 配置CUDA网格和块
    dim3 blockSize(16, 16);
    dim3 gridSize((width + blockSize.x - 1) / blockSize.x, 
                 (height + blockSize.y - 1) / blockSize.y);
    
    // 启动CUDA核函数
    mandelbrotKernel<<<gridSize, blockSize>>>(d_result, xMin, yMin, xStep, yStep, 
                                            width, height, maxIterations);
    
    // 复制结果回主机
    cudaMemcpy(result, d_result, size, cudaMemcpyDeviceToHost);
    
    // 释放设备内存
    cudaFree(d_result);
}