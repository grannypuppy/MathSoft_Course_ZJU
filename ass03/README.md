# 函数抽象类项目

这个项目实现了一个函数的抽象基类`Function`，并在此基础上派生出了`LinearFunction`类，通过模板将其类型一般化，支持多种数据类型。此外，还实现了`Polynomial`类来支持多项式函数。

## 项目结构

```
.
├── README.md          # 本文件
├── Makefile           # 编译配置文件
├── include/
│   ├── Function.hpp   # 函数抽象基类
│   ├── LinearFunction.hpp  # 线性函数类
│   └── Polynomial.hpp # 多项式函数类（加分功能）
└── src/
    └── test.cpp       # 测试代码
```

## 功能概述

1. **Function.hpp**
   - 定义了函数的抽象基类
   - 包含纯虚函数`operator()`用于计算函数值
   - 包含虚析构函数确保正确析构派生类

2. **LinearFunction.hpp**
   - 实现了线性函数 f(x) = ax + b
   - 通过模板支持多种数据类型
   - 提供获取斜率和截距的方法

3. **Polynomial.hpp**
   - 实现了多项式函数 f(x) = a_n * x^n + ... + a_1 * x + a_0
   - 使用秦九韶算法高效计算多项式值
   - 支持与LinearFunction相同的数据类型

## 支持的数据类型

- 整数类型 (int)
- 浮点类型 (float, double)
- 复数类型 (std::complex<double>)
- 向量类型 (std::vector<double>)
- GMP高精度浮点数 (mpf_class) - 需要GMP库支持

## 编译与运行

### 基本编译（不使用GMP库）

```bash
make
```

### 使用GMP库编译

```bash
make gmp
```

注意：使用GMP库编译需要先安装GMP开发库：

```bash
sudo apt install libgmp-dev
```

### 运行测试程序

```bash
./test
```

### 清理编译文件

```bash
make clean
```

## GMP库支持

本项目支持使用GNU Multiple Precision Arithmetic Library (GMP)进行高精度计算。GMP库提供了任意精度的算术运算，可以处理超出普通数据类型范围的大数值和高精度计算。

在本项目中，通过`mpf_class`类型支持GMP高精度浮点数，可以指定任意精度进行计算。

## 多项式函数实现

`Polynomial`类实现了任意次数的多项式函数，支持与`LinearFunction`相同的数据类型。多项式函数使用秦九韶算法(Horner's method)高效计算函数值，避免了直接计算幂次带来的性能问题。

## 测试示例

`test.cpp`文件包含了各种数据类型的测试示例，展示了如何使用`LinearFunction`和`Polynomial`类。测试覆盖了所有支持的数据类型，包括：

- 整数类型测试
- 浮点类型测试
- 复数类型测试
- 向量类型测试
- GMP高精度浮点数测试（如果启用）

