#include "include/mandelbrot.h"
#include <iostream>

// 声明CUDA函数
extern "C" void computeMandelbrotCUDA(int* result, double xMin, double yMin, double xMax, double yMax,
                                    int width, int height, int maxIterations);

int MandelbrotSet::computeIterations(const std::complex<double>& c, int maxIterations) {
    std::complex<double> z(0, 0);
    int iterations = 0;
    
    // 迭代计算 z = z^2 + c
    // 如果 |z| > 2，则点不在 Mandelbrot 集中
    while (std::abs(z) <= 2.0 && iterations < maxIterations) {
        z = z * z + c;
        iterations++;
    }
    
    return iterations;
}

std::vector<std::vector<int>> MandelbrotSet::computeSet(
    double xMin, double yMin, double xMax, double yMax,
    int width, int height, int maxIterations) {
    
    std::vector<std::vector<int>> result(height, std::vector<int>(width));
    
    double xStep = (xMax - xMin) / width;
    double yStep = (yMax - yMin) / height;
    
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            double real = xMin + x * xStep;
            double imag = yMin + y * yStep;
            std::complex<double> c(real, imag);
            
            result[y][x] = computeIterations(c, maxIterations);
        }
    }
    
    return result;
}

std::vector<std::vector<int>> MandelbrotSet::computeSetCUDA(
    double xMin, double yMin, double xMax, double yMax,
    int width, int height, int maxIterations) {
    
    // 创建一维结果数组
    int* flatResult = new int[width * height];
    
    try {
        // 调用CUDA函数
        computeMandelbrotCUDA(flatResult, xMin, yMin, xMax, yMax, width, height, maxIterations);
        
        // 将一维结果转换为二维向量
        std::vector<std::vector<int>> result(height, std::vector<int>(width));
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                result[y][x] = flatResult[y * width + x];
            }
        }
        
        delete[] flatResult;
        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "CUDA execution error: " << e.what() << std::endl;
        std::cerr << "Falling back to CPU implementation" << std::endl;
        delete[] flatResult;
        
        // 如果CUDA失败，回退到CPU实现
        return computeSet(xMin, yMin, xMax, yMax, width, height, maxIterations);
    }
}