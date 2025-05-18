CC = gcc
CFLAGS = -Wall -O3 -std=c99 -lm
SRC_DIR = src
BIN_DIR = bin
EXPORT_DIR = ExportPara

all: directories mnist_cnn

directories:
	mkdir -p $(BIN_DIR)

# 编译CNN推理程序
mnist_cnn: $(SRC_DIR)/mnist_cnn.c
	$(CC) $(SRC_DIR)/mnist_cnn.c -o $(BIN_DIR)/mnist_cnn $(CFLAGS)

# 清理编译产物
clean:
	rm -rf $(BIN_DIR)

# 运行CNN推理（无参数：测试整个测试集）
run:
	$(BIN_DIR)/mnist_cnn

# 运行（第）一个测试图片的识别
test: 
	$(BIN_DIR)/mnist_cnn images/test_0.bmp

# 测试所有图片
test_all:
	for i in `seq 0 9`; do \
		echo "Testing images/test_$$i.bmp:"; \
		$(BIN_DIR)/mnist_cnn images/test_$$i.bmp; \
	done

# 导出CNN参数
export_cnn:
	jupyter nbconvert --execute $(EXPORT_DIR)/export_cnn.ipynb
	
.PHONY: all clean run test test_all gen_test_images directories export_cnn
