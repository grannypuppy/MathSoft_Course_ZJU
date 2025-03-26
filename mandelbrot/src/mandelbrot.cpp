#include "include/mandelbrot.h"

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
