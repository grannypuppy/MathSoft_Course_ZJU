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
│   └── mnist_cnn.c             # CNN前向推理的C语言实现
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

> 使用顺序：
> ```shell
> make
> make export_cnn
> make run
> make test_all
> ```

## 技术细节

### CNN网络结构

#### 原来提供的参考网络使用的CNN结构如下：

1. 卷积层1: 输入1×28×28，输出32×28×28，卷积核大小3×3，padding=1
2. ReLU激活函数
3. 最大池化: 2×2，输出32×14×14
4. 卷积层2: 输入32×14×14，输出64×14×14，卷积核大小3×3，padding=1
5. ReLU激活函数
6. 最大池化: 2×2，输出64×7×7
7. 全连接层1: 输入64×7×7=3136，输出128
8. ReLU激活函数
9. 全连接层2: 输入128，输出10

> 总参数量为：  32x1x3x3 + 32 + 64x32x3x3 + 64 + 64x7x7x128 + 128 + 128x10 + 10 = 421,642
> 正确率：99.0%

#### 我经过简化后的参数网络结构如下：

1. 卷积层1: 输入1×28×28，输出2×28×28，卷积核大小3×3，padding=1
2. ReLU激活函数
3. 最大池化: 2×2，输出2×14×14
4. 卷积层2: 输入2×14×14，输出4×14×14，卷积核大小3×3，padding=1
5. ReLU激活函数
6. 最大池化: 2×2，输出4×7×7
7. 全连接层1: 输入4×7×7=196，输出24
8. ReLU激活函数
9. 全连接层2: 输入24，输出10

> 总参数量为：2x1x3x3 + 2 + 4x2x3x3 + 4 + 4x7x7x24 + 24 + 24x10 + 10 = 5,074
> MNIST验证集正确率：97.4%

***参数规模缩小了80倍，正确率仅下降了1.6%***

#### 自己的手写数字测验结果

```shell
bin/mnist_cnn images/test_0.bmp
识别结果: 0

bin/mnist_cnn images/test_1.bmp
识别结果: 1

bin/mnist_cnn images/test_2.bmp
识别结果: 2

bin/mnist_cnn images/test_3.bmp
识别结果: 3

bin/mnist_cnn images/test_4.bmp
识别结果: 1

bin/mnist_cnn images/test_5.bmp
识别结果: 5

bin/mnist_cnn images/test_6.bmp
识别结果: 6

bin/mnist_cnn images/test_7.bmp
识别结果: 1

bin/mnist_cnn images/test_8.bmp
识别结果: 8

bin/mnist_cnn images/test_9.bmp
识别结果: 1
```

正确率：70%，其中不论0-9哪个的手写图输入，tag1结果均处于较高数值

> 并且，随着我不断减小网络规模和复杂度，尽管MNIST验证集的正确率缓慢下降，但手写结果的正确率显著提高（从20%提高到70%）

> 可能是由于任务过于简单，MNIST数据集过于典型，发生了很强的过拟合，后续我尝试调整