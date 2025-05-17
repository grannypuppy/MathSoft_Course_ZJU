CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I./include -lm
TARGET = mnist_cnn
SRC_DIR = src
BUILD_DIR = build
DATA_DIR = data
BIN_DIR = bin

# 创建必要的目录
$(shell mkdir -p $(BUILD_DIR) $(BIN_DIR) $(DATA_DIR))

# 源文件和目标文件
SRC = $(SRC_DIR)/mnist_cnn.c
OBJ = $(BUILD_DIR)/mnist_cnn.o
BIN = $(BIN_DIR)/$(TARGET)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS)

$(OBJ): $(SRC)
	$(CC) -c $< -o $@ $(CFLAGS)

# 检查数据集是否存在，如果不存在则给出提示
check_data:
	@if [ ! -f "$(DATA_DIR)/t10k-images-idx3-ubyte" ] || [ ! -f "$(DATA_DIR)/t10k-labels-idx1-ubyte" ]; then \
		echo "测试数据集不存在，请确保 $(DATA_DIR) 目录下有 t10k-images-idx3-ubyte 和 t10k-labels-idx1-ubyte 文件"; \
		exit 1; \
	fi

# 测试模型在MNIST测试集上的准确率
test: $(BIN) check_data
	./$(BIN)

# 使用模型识别一个BMP图像
recognize: $(BIN)
	@if [ -z "$(IMAGE)" ]; then \
		echo "未指定图像文件，请使用 make recognize IMAGE=path/to/image.bmp"; \
	else \
		./$(BIN) $(IMAGE); \
	fi

# 识别images目录下的所有BMP图像
recognize_all: $(BIN)
	@for file in images/*.bmp; do \
		echo "识别图像: $$file"; \
		./$(BIN) $$file; \
	done

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean test recognize recognize_all check_data
