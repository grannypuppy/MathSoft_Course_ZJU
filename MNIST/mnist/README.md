# MNIST CNN 手写数字识别

本项目实现了一个基于卷积神经网络(CNN)的手写数字识别系统，包括:
1. 使用PyTorch训练CNN模型并导出参数
2. 使用C语言实现CNN的前向推理
3. 对手写数字图片进行识别

## 项目完成情况

- [x] 基本要求：导出CNN参数、手写数字、C语言推理、Makefile构建
- [x] 优化要求：调整网络和参数，提升推理效率

## 目录结构

```
mnist/
├── README.md                   # 项目说明文档
├── Makefile                    # 项目构建文件
├── ExportPara/                 # 参数导出代码
│   ├── parameters_cnn.bin          # CNN网络参数文件
│   └── export_cnn.ipynb        # 用于导出CNN参数的Jupyter Notebook
├── images/                     # 手写数字测试图片
│   ├── test_0.bmp              # 数字0的测试图片
│   ├── test_1.bmp              # 数字1的测试图片
│   └── ...                     # 其他数字的测试图片
├── src/                        # 源代码
│   ├── mnist_cnn.c             # CNN前向推理的C语言实现
│   └── generate_test_images.py # 生成测试图片的Python脚本
└── bin/                        # 编译产物目录
    └── mnist_cnn               # 编译后的可执行文件
```

## 如何使用

### 1. 编译项目

```bash
make
```

### 2. 导出CNN参数

```bash
make export_cnn
```

### 3. 测试单个图片

```bash
make test
```

或者指定特定的图片：

```bash
./bin/mnist_cnn images/test_5.bmp
```

### 4. 测试所有示例图片

```bash
make test_all
```

### 5. 在MNIST测试集上评估准确率

```bash
make run
```

### 6. 清理编译产物

```bash
make clean
```

## 技术细节

### CNN网络结构

本项目使用的CNN结构如下：

1. 卷积层1: 输入1×28×28，输出32×28×28，卷积核大小3×3，padding=1
2. ReLU激活函数
3. 最大池化: 2×2，输出32×14×14
4. 卷积层2: 输入32×14×14，输出64×14×14，卷积核大小3×3，padding=1
5. ReLU激活函数
6. 最大池化: 2×2，输出64×7×7
7. 全连接层1: 输入64×7×7=3136，输出128
8. ReLU激活函数
9. 全连接层2: 输入128，输出10

### 性能优化

为了提高C语言推理的效率，本项目使用了以下优化技术：

1. 使用线性内存布局减少内存访问开销
2. 避免动态内存分配和释放的频繁操作
3. 卷积核实现时直接计算偏移量，避免多重循环中的重复计算
