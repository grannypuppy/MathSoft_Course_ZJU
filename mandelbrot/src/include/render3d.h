#pragma once

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

class Render3D {
public:
    // 创建3D渲染的Mandelbrot集
    static bool create3DRender(const std::vector<std::vector<int>>& data,
                              const std::string& filename,
                              int maxIterations,
                              double heightScale = 0.15);
    
    // 创建3D旋转动画
    static bool create3DAnimation(const std::vector<std::vector<int>>& data,
                                 const std::string& filename,
                                 int maxIterations,
                                 int frames = 60,
                                 double heightScale = 0.15);
    
private:
    // 生成3D高度图
    static cv::Mat generateHeightMap(const std::vector<std::vector<int>>& data,
                                    int maxIterations);
    
    // 渲染3D视图
    static cv::Mat render3DView(const cv::Mat& heightMap,
                               const cv::Mat& colorMap,
                               double angleX, double angleY, double angleZ,
                               double heightScale);
};

