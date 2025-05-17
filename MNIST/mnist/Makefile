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

# 生成测试图片
gen_test_images:
	python $(SRC_DIR)/generate_test_images.py

# 清理编译产物
clean:
	rm -rf $(BIN_DIR)

# 运行CNN推理（无参数：测试整个测试集）
run:
	cp -n MNIST_student/data/t10k-images-idx3-ubyte data/ 2>/dev/null || true
	cp -n MNIST_student/data/t10k-labels-idx1-ubyte data/ 2>/dev/null || true
	$(BIN_DIR)/mnist_cnn

# 运行一个测试图片的识别
test: 
	$(BIN_DIR)/mnist_cnn images/test_0.bmp

# 测试所有图片
test_all:
	for i in {0..9}; do \
		echo "Testing images/test_$$i.bmp:"; \
		$(BIN_DIR)/mnist_cnn images/test_$$i.bmp; \
	done

# 导出CNN参数
export_cnn:
	mkdir -p data
	jupyter nbconvert --to notebook --execute $(EXPORT_DIR)/export_cnn.ipynb --output export_cnn_executed
	mv parameters_cnn.bin ./
	
.PHONY: all clean run test test_all gen_test_images directories export_cnn
