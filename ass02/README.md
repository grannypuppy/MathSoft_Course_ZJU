# Conway's Game of Life

## 项目概述

这是一个基于C++和ncurses库实现的Conway生命游戏。Conway生命游戏是由英国数学家John Conway在1970年发明的零玩家细胞自动机，通过简单的规则产生复杂的演化模式。

### 游戏规则

1. **孤独致死**：任何活细胞周围有少于2个活细胞的话会死亡
2. **生存**：任何活细胞周围有2或3个活细胞的话保持活着
3. **拥挤致死**：任何活细胞周围有超过3个活细胞的话会死亡
4. **繁殖**：任何死细胞周围正好有3个活细胞的话会变成活细胞

## 功能特性

- **基于终端的可视化界面**：使用ncurses库实现彩色终端显示
- **多种初始模式**：
  - 随机模式
  - 滑翔机模式（Glider）
  - 闪烁器模式（Blinker）
  - 高斯帕滑翔机枪模式（Gosper Glider Gun）
- **交互控制**：
  - `q` - 退出游戏
  - `s` - 将当前状态保存为BMP图像
  - `r` - 随机重置游戏状态
- **图像保存功能**：可将当前游戏状态导出为BMP格式图像
- **完整文档**：包含Doxygen生成的HTML和PDF格式文档

## 依赖安装

### Ubuntu/Debian
```bash
# 更新包列表
sudo apt update

# 安装编译工具和库
sudo apt-get install -y build-essential libncurses5-dev

# 安装Doxygen（用于生成文档）
sudo apt install -y doxygen graphviz

```
## 项目结构

```
ass02/
├── GameOfLife.cpp      # 游戏逻辑实现
├── GameOfLife.h        # 头文件（含Doxygen注释）
├── main.cpp            # 主程序入口
├── Makefile            # 构建系统配置
├── Doxyfile            # Doxygen配置文件
├── README.md           # 项目说明文档
├── html/               # 生成的HTML文档（运行make doc后）
├── latex/              # 生成的LaTeX文档（运行make doc后）
└── GameOfLife_documentation.pdf  # 生成的PDF文档（运行make doc后）
```

## 编译项目

```bash
# 编译游戏
make
# 编译并生成文档
make doc
```

## 运行游戏

### 直接运行

```bash
# 从项目目录运行
./game_of_life
```

### 游戏界面说明

游戏启动后，会显示一个菜单：

```shell
Conway's Game of Life
====================
1. Random pattern
2. Glider pattern
3. Blinker pattern
4. Gosper glider gun pattern
0. Exit
Enter your choice:
```

选择一个初始模式后，游戏将在终端中运行。屏幕底部会显示当前代数和控制按键说明。

#### 控制按键
- `q` 或 `Q`: 退出游戏
- `s` 或 `S`: 保存当前状态为BMP图像
- `r` 或 `R`: 重置为随机状态

## 清理项目

```bash
# 清理编译生成的文件
make clean

# 清理生成的文档
make clean-doc

# 清理所有生成的文件
make distclean

```

## 功能

### 修改游戏尺寸
默认情况下，游戏使用30x80的网格。如需修改，请编辑`main.cpp`文件中的以下部分：

```cpp
// 修改这些值以改变游戏网格尺寸
int height = 30;  // 网格高度
int width = 80;   // 网格宽度
```

## 图像保存功能

按下`s`键可以将当前游戏状态保存为BMP图像。图像将保存在当前目录下，文件名格式为`gameoflife_YYYYMMDD_HHMMSS.bmp`，其中包含保存时的时间戳。

保存的图像使用以下颜色表示：

- 活细胞：绿色
- 死细胞：黑色

---

*"生命游戏不是为了赢，而是为了观察生命的演化。" - John Conway*