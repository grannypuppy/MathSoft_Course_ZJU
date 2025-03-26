# Mandelbrot 集可视化项目

这个项目实现了 Mandelbrot 集的计算与可视化，使用 C++ 开发，支持多种输出格式和视觉效果。项目支持 CPU 和 CUDA GPU 加速计算，可以生成不同颜色映射的静态图像以及缩放动画。

## 加分点

- [x] 早于第六周提交;
- [x] 代码结构更加清晰, 有良好的注释和文档;
- [x] 有良好的项目管理, 使用 Makefile, CMake等构建工具（请在文档README.md中说明）;
- [x] 采用其他图片格式增加了展示效果;
- [x] 提供色彩更加丰富的可视化结果;
- [x] 提供展现局部方法的动态图形或视频;
- [ ] 其他更加丰富的可视化表现，如 3D 渲染;
- [ ] \*数学上对 Mandelbrot 集进行分析和探索, 增强 report 的学术份量;
- [x] \*提升计算效率，例如利用并行计算，GPU 加速等;
- [ ] \*其他创意、数学、算法和技术上的提升。

## 项目结构

```shell
mandelbrot/
├── src/
│   ├── include/
│   │   ├── mandelbrot.h    # Mandelbrot 计算相关声明
│   │   └── image.h         # 图像处理相关声明
│   ├── mandelbrot.cpp      # Mandelbrot 集计算实现
│   ├── mandelbrot_cuda.cu  # CUDA 加速版本实现
│   ├── image.cpp           # 图像生成和处理实现
│   └── test.cpp            # 主程序入口
├── doc/                   # 文档目录
│   ├── report.tex        # LaTeX 报告源文件
│   ├── report.pdf        # 生成的PDF报告
│   └── figures/          # 报告中使用的图片
├── build/                  # 编译生成的目标文件
├── Makefile                # 项目编译配置
└── README.md               # 项目说明文档
```

## 依赖项

本项目依赖以下库和工具：

1. **C++ 编译器**：支持 C++11 标准
2. **NVIDIA CUDA Toolkit**：用于 GPU 加速计算（可选）
3. **OpenCV 4.x**：用于图像处理、颜色映射和保存 PNG 格式图像
4. **FFmpeg**：用于生成 GIF 动画

### 依赖安装

#### Ubuntu:

```bash
# 安装 C++ 开发工具/Make 等工具过程省略，展示额外依赖

# 安装 OpenCV
sudo apt-get install libopencv-dev
# 安装 FFmpeg
sudo apt-get install ffmpeg

# 安装 CUDA Toolkit（可选，用于 GPU 加速）
sudo apt install -y nvidia-cuda-toolkit
# 对于该功能需要有NVIDDIA显卡，请访问 NVIDIA 官网下载适合您系统的 CUDA 驱动
# 而且对于windows系统，如果是在wsl2中运行，需要配置wsl2的gpu访问权限
```

##### Cuda on WSL2

- 请访问 [NVIDIA CUDA 下载页面](https://developer.nvidia.com/cuda-downloads)
- 选择适合您 Windows 版本的安装包
- 下载并运行安装程序，按照向导完成安装
- 安装过程会同时安装 CUDA Toolkit 和兼容的 NVIDIA 驱动

###### WSL2 特殊配置

如果您在 Windows WSL2 中运行 Linux 系统：

1. **更新 WSL2**：确保使用最新版本的 WSL2

   ```powershell
   wsl --update
   ```

2. **配置 WSL2 的 GPU 访问权限**：
   - 在 Windows 主机上安装最新的 [NVIDIA 驱动程序](https://www.nvidia.com/Download/index.aspx)
   - 在 `%UserProfile%\.wslconfig` 文件中添加以下配置：

     ```
     [wsl2]
     gpuSupport=true
     ```

   - 重启 WSL2：

     ```powershell
     wsl --shutdown
     ```

3. **在 WSL2 中验证 GPU 访问**：

   ```bash
   nvidia-smi
   ```

##### 验证安装

安装完成后，请运行以下命令验证安装是否成功：

```bash
# 检查 CUDA 版本
nvcc --version

# 检查 GPU 状态和驱动版本
nvidia-smi
```

## 编译项目

使用 Make 编译项目：

```bash
make
```

## 项目功能

1. **基础计算**：计算给定复平面区域内的 Mandelbrot 集
2. **多格式输出**：支持保存为 PPM 和 PNG 图像
3. **颜色映射**：
   - HSV 颜色空间平滑渐变
   - 基于正弦函数的周期性颜色映射
4. **高性能计算**：支持 CUDA GPU 加速
5. **动态缩放**：生成 Mandelbrot 集缩放动画

## 使用方法

### report 生成
```shell
# 在makefile所在目录下运行
make report
```
### 基本命令

```bash
./mandelbrot [选项]
```

### 选项说明

- `--basic`：生成基本 PPM 格式图像（默认模式）
- `--png [s]`：生成 PNG 格式图像，使用增强颜色
  - 添加 `s` 参数使用更鲜艳的 HSV 颜色映射（例如：`--png s`）
  - 不添加参数时使用正弦波颜色映射
- `--zoom`：生成 Mandelbrot 集缩放动画（GIF 格式）
- `--cuda`：使用 CUDA GPU 加速计算（若可用）
- `--help`：显示帮助信息

### 示例命令

```bash
# 生成基本 PPM 图像
./mandelbrot --basic

# 生成使用正弦波颜色映射的 PNG 图像
./mandelbrot --png

# 生成使用平滑 HSV 颜色映射的 PNG 图像
./mandelbrot --png s

# 使用 CUDA 加速生成 PNG 图像
./mandelbrot --cuda --png

# 生成缩放动画
./mandelbrot --zoom

# 使用 CUDA 加速生成缩放动画
./mandelbrot --cuda --zoom
```

### Make 快捷命令

项目提供了多个 Make 快捷命令用于运行不同模式：

```bash
make report       #编译report.tex 生成report
make run          # 运行默认模式
make run-basic    # 生成基本 PPM 图像
make run-png      # 生成 PNG 图像（正弦波颜色映射）
make run-zoom     # 生成缩放动画
make run-cuda     # 使用 CUDA 加速运行默认模式
make run-cuda-png # 使用 CUDA 加速生成 PNG 图像
make run-cuda-zoom # 使用 CUDA 加速生成缩放动画
```

## 输出文件

- report.pdf
- mandelbrot.ppm：基本 PPM 格式图像
- mandelbrot.png：增强颜色的 PNG 格式图像(有两种颜色风格可以尝试)
- mandelbrot_zoom.gif：缩放动画 GIF

## 其他的一些说明

- **多分辨率支持**：可以通过修改代码中的 `width` 和 `height` 变量调整输出图像分辨率
- **CUDA 加速**：利用 GPU 并行计算加速 Mandelbrot 集的生成，特别适合高分辨率图像和高迭代次数
- **回退机制**：如果 CUDA 不可用，程序会自动回退到 CPU 实现

## 清理项目

```bash
make clean-latex #清理latex编译的中间文件
make clean
```
