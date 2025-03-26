#include "include/image.h"
#include "include/mandelbrot.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

bool Image::saveAsPPM(const std::vector<std::vector<int>>& data, 
                     const std::string& filename,
                     int maxIterations) {
    
    if (data.empty() || data[0].empty()) {
        std::cerr << "Empty data provided" << std::endl;
        return false;
    }
    
    int height = data.size();
    int width = data[0].size();
    
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }
    
    // PPM 文件头
    file << "P3\n" << width << " " << height << "\n255\n";
    
    // 写入像素数据
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int iterations = data[y][x];
            
            // 如果点在 Mandelbrot 集中（达到最大迭代次数），则为黑色
            if (iterations == maxIterations) {
                file << "0 0 0 ";
            } else {
                // 根据迭代次数生成颜色
                // 使用平滑的颜色渐变
                double ratio = static_cast<double>(iterations) / maxIterations;
                int r = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 3.14159)));
                int g = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 6.28318)));
                int b = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 9.42477)));
                
                file << r << " " << g << " " << b << " ";
            }
        }
        file << "\n";
    }
    
    return true;
}

cv::Mat Image::createColorfulImage(const std::vector<std::vector<int>>& data,
                                 int maxIterations,
                                 bool useSmoothing) {
    if (data.empty() || data[0].empty()) {
        std::cerr << "Empty data provided" << std::endl;
        return cv::Mat();
    }
    
    int height = data.size();
    int width = data[0].size();
    
    cv::Mat image(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
    
    // 创建一个颜色映射表，用于更丰富的颜色效果
    std::vector<cv::Vec3b> colorMap(maxIterations + 1);
    
    // 生成颜色映射表
    for (int i = 0; i <= maxIterations; i++) {
        double ratio = static_cast<double>(i) / maxIterations;
        
        if (i == maxIterations) {
            // 在集合内部的点为黑色
            colorMap[i] = cv::Vec3b(0, 0, 0);
        } else if (useSmoothing) {
            // 使用更丰富的颜色映射
            double v = ratio * 6;
            int vi = static_cast<int>(v);
            double vf = v - vi;
            
            double r, g, b;
            switch (vi % 6) {
                case 0: r = 1.0; g = vf; b = 0.0; break;
                case 1: r = 1.0 - vf; g = 1.0; b = 0.0; break;
                case 2: r = 0.0; g = 1.0; b = vf; break;
                case 3: r = 0.0; g = 1.0 - vf; b = 1.0; break;
                case 4: r = vf; g = 0.0; b = 1.0; break;
                case 5: r = 1.0; g = 0.0; b = 1.0 - vf; break;
            }
            
            // 应用对数缩放以突出细节
            double logScale = std::log(ratio * 9 + 1) / std::log(10);
            r = std::pow(r * 255, logScale);
            g = std::pow(g * 255, logScale);
            b = std::pow(b * 255, logScale);
            
            colorMap[i] = cv::Vec3b(static_cast<uchar>(b), static_cast<uchar>(g), static_cast<uchar>(r));
        } else {
            // 简单的颜色映射
            int r = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 3.14159)));
            int g = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 6.28318)));
            int b = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 9.42477)));
            
            colorMap[i] = cv::Vec3b(b, g, r); // OpenCV使用BGR顺序
        }
    }
    
    // 应用颜色映射到图像
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int iterations = data[y][x];
            image.at<cv::Vec3b>(y, x) = colorMap[iterations];
        }
    }
    
    return image;
}

bool Image::saveImage(const std::vector<std::vector<int>>& data,
                     const std::string& filename,
                     int maxIterations,
                     bool useSmoothing) {
    
    cv::Mat image = createColorfulImage(data, maxIterations, useSmoothing);
    if (image.empty()) {
        return false;
    }
    
    // 保存图像
    try {
        cv::imwrite(filename, image);
        return true;
    } catch (const cv::Exception& ex) {
        std::cerr << "Error saving image: " << ex.what() << std::endl;
        return false;
    }
}

bool Image::createZoomGif(double centerX, double centerY,
                         double startScale, double endScale,
                         int frames, int width, int height,
                         const std::string& filename,
                         int maxIterations) {
    
    // 创建一个临时目录来存储帧
    std::string tempDir = "temp_frames";
    system(("mkdir -p " + tempDir).c_str());
    
    // 计算每一帧的缩放比例（使用对数缩放以获得平滑的缩放效果）
    std::vector<double> scales(frames);
    for (int i = 0; i < frames; i++) {
        double t = static_cast<double>(i) / (frames - 1);
        scales[i] = startScale * std::pow(endScale / startScale, t);
    }
    
    // 生成每一帧
    for (int i = 0; i < frames; i++) {
        double scale = scales[i];
        double xMin = centerX - scale;
        double xMax = centerX + scale;
        double yMin = centerY - scale * height / width;
        double yMax = centerY + scale * height / width;
        
        std::cout << "Generating frame " << (i+1) << "/" << frames 
                  << " (scale: " << scale << ")" << std::endl;
        
        auto result = MandelbrotSet::computeSet(xMin, yMin, xMax, yMax, 
                                               width, height, maxIterations);
        
        std::string frameFilename = tempDir + "/frame_" + 
                                   std::to_string(i) + ".png";
        saveImage(result, frameFilename, maxIterations, true);
    }
    
    // 使用ffmpeg将帧合成为GIF
    std::string cmd = "ffmpeg -y -framerate 10 -i " + tempDir + 
                     "/frame_%d.png -vf \"fps=10,scale=trunc(iw/2)*2:trunc(ih/2)*2,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse\" " + 
                     filename;
    
    int result = system(cmd.c_str());
    
    // 清理临时文件
    system(("rm -rf " + tempDir).c_str());
    
    return result == 0;
}
