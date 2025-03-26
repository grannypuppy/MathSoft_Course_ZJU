#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

class Image {
public:
    // 从迭代数据创建图像
    static bool saveAsPPM(const std::vector<std::vector<int>>& data, 
                         const std::string& filename,
                         int maxIterations);
    
    // 新增：使用OpenCV保存为多种格式
    static bool saveImage(const std::vector<std::vector<int>>& data,
                         const std::string& filename,
                         int maxIterations,
                         bool useSmoothing = true);
    
    // 新增：生成高质量彩色图像
    static cv::Mat createColorfulImage(const std::vector<std::vector<int>>& data,
                                     int maxIterations,
                                     bool useSmoothing = true);
    
    // 新增：生成动态缩放效果的GIF
    static bool createZoomGif(double centerX, double centerY,
                             double startScale, double endScale,
                             int frames, int width, int height,
                             const std::string& filename,
                             int maxIterations);
};

