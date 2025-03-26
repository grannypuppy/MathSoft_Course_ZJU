#include "include/render3d.h"
#include "include/image.h"
#include <cmath>
#include <iostream>

cv::Mat Render3D::generateHeightMap(const std::vector<std::vector<int>>& data,
                                  int maxIterations) {
    if (data.empty() || data[0].empty()) {
        return cv::Mat();
    }
    
    int height = data.size();
    int width = data[0].size();
    
    cv::Mat heightMap(height, width, CV_32F, cv::Scalar(0));
    
    // 将迭代次数转换为高度值
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int iterations = data[y][x];
            
            if (iterations == maxIterations) {
                // 在集合内部的点为0高度
                heightMap.at<float>(y, x) = 0.0f;
            } else {
                // 使用平滑的高度映射
                float ratio = static_cast<float>(iterations) / maxIterations;
                // 使用对数缩放以突出细节
                heightMap.at<float>(y, x) = std::log(1 + ratio * 9) / std::log(10);
            }
        }
    }
    
    // 应用高斯模糊使表面更平滑
    cv::GaussianBlur(heightMap, heightMap, cv::Size(3, 3), 0);
    
    return heightMap;
}

cv::Mat Render3D::render3DView(const cv::Mat& heightMap,
                             const cv::Mat& colorMap,
                             double angleX, double angleY, double angleZ,
                             double heightScale) {
    if (heightMap.empty() || colorMap.empty() || 
        heightMap.size() != colorMap.size()) {
        return cv::Mat();
    }
    
    int height = heightMap.rows;
    int width = heightMap.cols;
    
    // 创建输出图像
    cv::Mat output(height, width, CV_8UC3, cv::Scalar(0, 0, 0));
    
    // 计算旋转矩阵
    double cosX = std::cos(angleX);
    double sinX = std::sin(angleX);
    double cosY = std::cos(angleY);
    double sinY = std::sin(angleY);
    double cosZ = std::cos(angleZ);
    double sinZ = std::sin(angleZ);
    
    // 光源方向（简化的光照模型）
    cv::Vec3f lightDir(0.5, 0.5, 1.0);
    lightDir = cv::normalize(lightDir);
    
    // 渲染3D视图
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            // 获取当前点的高度
            float h = heightMap.at<float>(y, x) * heightScale;
            
            // 计算法线向量（使用相邻点估计）
            float hx1 = heightMap.at<float>(y, x+1) * heightScale;
            float hx2 = heightMap.at<float>(y, x-1) * heightScale;
            float hy1 = heightMap.at<float>(y+1, x) * heightScale;
            float hy2 = heightMap.at<float>(y-1, x) * heightScale;
            
            cv::Vec3f normal(-hx1 + hx2, -hy1 + hy2, 2.0);
            normal = cv::normalize(normal);
            
            // 应用旋转
            cv::Vec3f rotatedNormal;
            rotatedNormal[0] = normal[0] * cosY * cosZ - normal[1] * cosY * sinZ + normal[2] * sinY;
            rotatedNormal[1] = normal[0] * (sinX * sinY * cosZ + cosX * sinZ) + 
                             normal[1] * (cosX * cosZ - sinX * sinY * sinZ) - 
                             normal[2] * sinX * cosY;
            rotatedNormal[2] = normal[0] * (sinX * sinZ - cosX * sinY * cosZ) + 
                             normal[1] * (cosX * sinY * sinZ + sinX * cosZ) + 
                             normal[2] * cosX * cosY;
            
            // 计算光照强度（点积）
            float intensity = std::max(0.2f, std::abs(rotatedNormal.dot(lightDir)));
            
            // 获取基础颜色并应用光照
            cv::Vec3b color = colorMap.at<cv::Vec3b>(y, x);
            color[0] = cv::saturate_cast<uchar>(color[0] * intensity);
            color[1] = cv::saturate_cast<uchar>(color[1] * intensity);
            color[2] = cv::saturate_cast<uchar>(color[2] * intensity);
            
            output.at<cv::Vec3b>(y, x) = color;
        }
    }
    
    return output;
}

bool Render3D::create3DRender(const std::vector<std::vector<int>>& data,
                            const std::string& filename,
                            int maxIterations,
                            double heightScale) {
    // 生成高度图
    cv::Mat heightMap = generateHeightMap(data, maxIterations);
    if (heightMap.empty()) {
        std::cerr << "Failed to generate height map" << std::endl;
        return false;
    }
    
    // 生成颜色图
    cv::Mat colorMap = Image::createColorfulImage(data, maxIterations, true);
    if (colorMap.empty()) {
        std::cerr << "Failed to generate color map" << std::endl;
        return false;
    }
    
    // 渲染3D视图
    cv::Mat output = render3DView(heightMap, colorMap, 
                                 M_PI / 6, 0, 0, heightScale);
    if (output.empty()) {
        std::cerr << "Failed to render 3D view" << std::endl;
        return false;
    }
    
    // 保存图像
    try {
        cv::imwrite(filename, output);
        return true;
    } catch (const cv::Exception& ex) {
        std::cerr << "Error saving 3D render: " << ex.what() << std::endl;
        return false;
    }
}

bool Render3D::create3DAnimation(const std::vector<std::vector<int>>& data,
                               const std::string& filename,
                               int maxIterations,
                               int frames,
                               double heightScale) {
    // 创建一个临时目录来存储帧
    std::string tempDir = "temp_frames";
    system(("mkdir -p " + tempDir).c_str());
    
    // 生成高度图
    cv::Mat heightMap = generateHeightMap(data, maxIterations);
    if (heightMap.empty()) {
        std::cerr << "Failed to generate height map" << std::endl;
        return false;
    }
    
    // 生成颜色图
    cv::Mat colorMap = Image::createColorfulImage(data, maxIterations, true);
    if (colorMap.empty()) {
        std::cerr << "Failed to generate color map" << std::endl;
        return false;
    }
    
    // 生成每一帧
    for (int i = 0; i < frames; i++) {
        double angle = 2 * M_PI * i / frames;
        
        std::cout << "Generating 3D frame " << (i+1) << "/" << frames << std::endl;
        
        // 渲染3D视图（旋转视图）
        cv::Mat output = render3DView(heightMap, colorMap, 
                                     M_PI / 6, angle, 0, heightScale);
        if (output.empty()) {
            std::cerr << "Failed to render 3D view for frame " << i << std::endl;
            continue;
        }
        
        // 保存帧
        std::string frameFilename = tempDir + "/frame_" + 
                                   std::to_string(i) + ".png";
        cv::imwrite(frameFilename, output);
    }
    
    // 使用ffmpeg将帧合成为视频
    std::string cmd = "ffmpeg -y -framerate 24 -i " + tempDir + 
                     "/frame_%d.png -c:v libx264 -pix_fmt yuv420p " + 
                     filename;
    
    int result = system(cmd.c_str());
    
    // 清理临时文件
    system(("rm -rf " + tempDir).c_str());
    
    return result == 0;
}
