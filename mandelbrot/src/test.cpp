#include "include/mandelbrot.h"
#include "include/image.h"
#include <iostream>
#include <chrono>
#include <string>

void printHelp() {
    std::cout << "Mandelbrot Set Visualization\n"
              << "Usage: ./mandelbrot [option]\n\n"
              << "Options:\n"
              << "  --basic       Generate basic Mandelbrot set image (default)\n"
              << "  --png [s]     Generate PNG image with enhanced colors\n"
              << "                Add 's' for smooth HSV coloring (e.g. --png s)\n"
              << "  --zoom        Generate zoom animation\n"
              << "  --help        Display this help message\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    // 默认参数
    double xMin = -1.5;
    double yMin = -1.0;
    double xMax = 1.5;
    double yMax = 1.0;
    int width = 800;
    int height = 600;
    int maxIterations = 1000;
    
    // 解析命令行参数
    std::string mode = "basic";
    bool useSmoothing = false;

    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--png") {
            mode = "png";
            // 检查是否有额外的平滑参数
            if (argc > 2 && argv[2][0] == 's') {
                useSmoothing = true;
            }
        }
        else if (arg == "--zoom") mode = "zoom";
        else if (arg == "--help") {
            printHelp();
            return 0;
        }
    }
    
    std::cout << "Computing Mandelbrot set for region: (" 
              << xMin << ", " << yMin << ") to (" 
              << xMax << ", " << yMax << ")" << std::endl;
    
    // 记录开始时间
    auto start = std::chrono::high_resolution_clock::now();
    
    // 计算 Mandelbrot 集
    auto result = MandelbrotSet::computeSet(xMin, yMin, xMax, yMax, 
                                           width, height, maxIterations);
    
    // 记录结束时间
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    
    std::cout << "Computation completed in " << elapsed.count() << " seconds" << std::endl;
    
    // 根据模式生成不同类型的输出
    if (mode == "basic") {
        // 保存为 PPM 图像
        std::string filename = "mandelbrot.ppm";
        if (Image::saveAsPPM(result, filename, maxIterations)) {
            std::cout << "Basic image saved as " << filename << std::endl;
        } else {
            std::cerr << "Failed to save basic image" << std::endl;
            return 1;
        }
    }
    else if (mode == "png") {
        // 保存为 PNG 图像，带增强的颜色
        std::string filename = "mandelbrot.png";
        if (Image::saveImage(result, filename, maxIterations, useSmoothing)) {
            std::cout << "Enhanced image saved as " << filename 
                      << (useSmoothing ? " (smooth HSV coloring)" : " (sine wave coloring)") 
                      << std::endl;
        } else {
            std::cerr << "Failed to save enhanced image" << std::endl;
            return 1;
        }
    }
    else if (mode == "zoom") {
        // 生成缩放动画
        std::string filename = "mandelbrot_zoom.gif";
        // 缩放到一个有趣的区域
        double centerX = -0.7436438870371587;
        double centerY = 0.1318259043691468;
        double startScale = 1.5;
        double endScale = 0.00001;
        int frames = 150;
        
        if (Image::createZoomGif(centerX, centerY, startScale, endScale, 
                                frames, width, height, filename, maxIterations)) {
            std::cout << "Zoom animation saved as " << filename << std::endl;
        } else {
            std::cerr << "Failed to create zoom animation" << std::endl;
            return 1;
        }
    }
    
    return 0;
}
