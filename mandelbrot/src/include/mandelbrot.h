#pragma once

#include <complex>
#include <vector>

class MandelbrotSet {
public:
    // 计算给定点是否属于 Mandelbrot 集，以及需要多少次迭代才能确定
    static int computeIterations(const std::complex<double>& c, int maxIterations);
    
    // 计算给定区域的 Mandelbrot 集
    static std::vector<std::vector<int>> computeSet(
        double xMin, double yMin, double xMax, double yMax,
        int width, int height, int maxIterations);
};


