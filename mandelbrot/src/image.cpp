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
                // 根据迭代次数生成颜色 - 修改为更明亮的颜色
                double ratio = static_cast<double>(iterations) / maxIterations;
                
                // 使用HSV颜色空间可以得到更鲜艳的结果
                int hue = static_cast<int>(360.0 * ratio);
                double saturation = 1.0;
                double value = 1.0;
                
                // 简单的HSV到RGB转换
                double c = value * saturation;
                double x = c * (1 - std::abs(std::fmod(hue / 60.0, 2) - 1));
                double m = value - c;
                
                double r, g, b;
                if (hue < 60) { r = c; g = x; b = 0; }
                else if (hue < 120) { r = x; g = c; b = 0; }
                else if (hue < 180) { r = 0; g = c; b = x; }
                else if (hue < 240) { r = 0; g = x; b = c; }
                else if (hue < 300) { r = x; g = 0; b = c; }
                else { r = c; g = 0; b = x; }
                
                int red = static_cast<int>(255 * (r + m));
                int green = static_cast<int>(255 * (g + m));
                int blue = static_cast<int>(255 * (b + m));
                
                file << red << " " << green << " " << blue << " ";
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
        if (i == maxIterations) {
            // 在集合内部的点为黑色
            colorMap[i] = cv::Vec3b(0, 0, 0);
        } else {
            // 使用更明亮的颜色映射
            double ratio = static_cast<double>(i) / maxIterations;
            
            if (useSmoothing) {
                // 使用HSV颜色空间生成更鲜艳的颜色
                double hue = 360.0 * ratio;
                cv::Mat hsv(1, 1, CV_8UC3, cv::Scalar(hue / 2, 255, 255)); // OpenCV的H范围是0-180
                cv::Mat rgb;
                cv::cvtColor(hsv, rgb, cv::COLOR_HSV2BGR);
                colorMap[i] = rgb.at<cv::Vec3b>(0, 0);
            } else {
                // 简单的颜色映射
                int r = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 3.14159)));
                int g = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 6.28318)));
                int b = static_cast<int>(255 * (0.5 + 0.5 * std::sin(ratio * 9.42477)));
                
                colorMap[i] = cv::Vec3b(b, g, r); // OpenCV使用BGR顺序
            }
        }
    }
    
    // 应用颜色映射到图像
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int iterations = data[y][x];
            image.at<cv::Vec3b>(y, x) = colorMap[iterations];
        }
    }
    
    // 增强对比度
    if (useSmoothing) {
        cv::Mat enhancedImage;
        cv::convertScaleAbs(image, enhancedImage, 1.2, 10);
        return enhancedImage;
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
    int mkdirResult = system(("mkdir -p " + tempDir).c_str());
    if (mkdirResult != 0) {
        std::cerr << "Failed to create temporary directory" << std::endl;
        return false;
    }
    
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
    
    int ffmpegResult = system(cmd.c_str());
    if (ffmpegResult != 0) {
        std::cerr << "Failed to create GIF using ffmpeg" << std::endl;
        // 即使ffmpeg失败，也尝试清理临时文件
    }
    
    // 清理临时文件
    int rmResult = system(("rm -rf " + tempDir).c_str());
    if (rmResult != 0) {
        std::cerr << "Warning: Failed to remove temporary directory" << std::endl;
    }
    
    return ffmpegResult == 0;
}
