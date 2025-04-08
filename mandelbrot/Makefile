CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++11 -Wall -O2
INCLUDES = -Isrc/include
LIBS = `pkg-config --libs opencv4`
CXXFLAGS += `pkg-config --cflags opencv4`

SRC_DIR = src
BUILD_DIR = build
TARGET = mandelbrot

# 源文件
CPP_SOURCES = $(SRC_DIR)/mandelbrot.cpp $(SRC_DIR)/image.cpp $(SRC_DIR)/test.cpp
CUDA_SOURCES = $(SRC_DIR)/mandelbrot_cuda.cu

# 目标文件
CPP_OBJECTS = $(CPP_SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
CUDA_OBJECTS = $(CUDA_SOURCES:$(SRC_DIR)/%.cu=$(BUILD_DIR)/%.o)

# 默认目标
all: $(TARGET)

# 创建构建目录
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# 编译C++源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# 编译CUDA源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cu | $(BUILD_DIR)
	$(NVCC) -c $< -o $@

# 链接规则
$(TARGET): $(CPP_OBJECTS) $(CUDA_OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS) -lcudart

# 运行测试
run: $(TARGET)
	./$(TARGET)

# 运行不同模式
run-basic: $(TARGET)
	./$(TARGET) --basic

run-png: $(TARGET)
	./$(TARGET) --png

run-zoom: $(TARGET)
	./$(TARGET) --zoom

run-cuda: $(TARGET)
	./$(TARGET) --cuda

run-cuda-png: $(TARGET)
	./$(TARGET) --cuda --png

run-cuda-zoom: $(TARGET)
	./$(TARGET) --cuda --zoom

# 编译LaTeX报告
report:
	cd doc && xelatex report.tex && cd ..

# 清理LaTeX中间文件
clean-latex:
	rm -f doc/*.aux doc/*.log doc/*.out doc/*.toc doc/*.synctex.gz

# 清理
clean: clean-latex
	rm -rf $(BUILD_DIR) $(TARGET) mandelbrot.ppm mandelbrot.png mandelbrot_zoom.gif

.PHONY: all run run-basic run-png run-zoom run-cuda run-cuda-png run-cuda-zoom clean clean-latex report