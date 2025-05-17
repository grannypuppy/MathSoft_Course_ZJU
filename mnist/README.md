# MNIST 手写数字识别

这是一个使用 CNN 进行 MNIST 手写数字识别的项目。项目包括两个主要部分：
1. 使用 PyTorch 训练 CNN 模型并导出参数
2. 使用 C 语言实现 CNN 模型的前向推理

## 目录结构

```
mnist/
├── README.md             # 本文件
├── Makefile              # 项目构建工具
├── images/               # 手写数字图片目录
│   └── test_0.bmp        # 手写数字图片（需自行添加）
│   └── ...
├── src/                  # 项目源代码
│   └── mnist_cnn.c       # C语言实现的CNN模型前向推理
├── ExportPara/           # 导出参数相关文件
│   └── export_cnn.ipynb  # 导出CNN模型参数的Jupyter Notebook
│   └── parameters_cnn.bin # 导出的CNN模型参数二进制文件
├── bin/                  # 编译后的可执行文件目录（会自动创建）
└── build/                # 编译产生的中间文件目录（会自动创建）
└── data/                 # 存放MNIST测试数据集
    └── t10k-images-idx3-ubyte
    └── t10k-labels-idx1-ubyte
```

## 已完成内容

- [x] 实现基本要求
  - [x] 利用Python导出CNN模型参数至二进制文件
  - [x] 通过C语言实现CNN模型前向推理
  - [x] 支持手写数字图片识别
  - [x] 支持测试集准确率评估
  - [x] 包含Makefile项目构建工具

## 使用方法

### 构建项目

```bash
# 在mnist目录下执行
make
```

### 测试模型准确率

```bash
# 在mnist目录下执行，评估模型在MNIST测试集上的准确率
make test
```

### 识别单张图片

```bash
# 识别单张图片
make recognize IMAGE=images/test_0.bmp
```

### 识别所有图片

```bash
# 识别images目录下的所有BMP图片
make recognize_all
```

### 清理编译产物

```bash
# 清理编译产生的文件
make clean
```

## 实现细节

### CNN 网络结构

该 CNN 模型包含以下层：
1. 第一个卷积层：1通道输入，32通道输出，3x3卷积核，填充为1
2. ReLU激活函数
3. 第二个卷积层：32通道输入，64通道输出，3x3卷积核，填充为1
4. ReLU激活函数
5. 最大池化层：2x2池化窗口，步长为2
6. 最大池化层：2x2池化窗口，步长为2
7. 第一个全连接层：64*7*7输入，128输出
8. ReLU激活函数
9. 第二个全连接层：128输入，10输出

### 模型参数导出

使用 `ExportPara/export_cnn.ipynb` 导出模型参数，参数将保存在 `ExportPara/parameters_cnn.bin` 中。

### C语言实现

C语言实现的前向推理支持两种模式：
1. 无参数模式：评估模型在MNIST测试集上的准确率
2. 有参数模式：识别指定的BMP图片

## 注意事项

1. 手写数字图片必须是28x28像素的24位BMP格式
2. 图片应为黑底白字（白色表示笔迹，黑色表示背景）
3. 使用前请确保已经运行了导出模型参数的Jupyter Notebook，并生成了参数文件

## 优化说明

为了提高C程序的推理效率，本项目进行了以下优化：
1. 在编译时使用 `-O2` 优化级别
2. 在CNN实现中，尽量避免重复计算和内存分配
3. 采用合理的数据结构来存储和处理中间结果
