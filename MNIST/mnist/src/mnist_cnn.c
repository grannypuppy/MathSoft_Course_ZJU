#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// 参数维度常量
#define IMG_W 28
#define IMG_H 28
#define IMG_C 1

// 卷积层1参数
#define CONV1_IN_CHANNELS 1
#define CONV1_OUT_CHANNELS 32
#define CONV1_KERNEL_SIZE 3
#define CONV1_PADDING 1
#define CONV1_OUT_W 28
#define CONV1_OUT_H 28

// 卷积层2参数
#define CONV2_IN_CHANNELS 32
#define CONV2_OUT_CHANNELS 64
#define CONV2_KERNEL_SIZE 3
#define CONV2_PADDING 1
#define CONV2_OUT_W 28
#define CONV2_OUT_H 28

// 池化后的尺寸
#define POOL1_OUT_W 14
#define POOL1_OUT_H 14
#define POOL2_OUT_W 7
#define POOL2_OUT_H 7

// 全连接层参数
#define FC1_IN 64 * 7 * 7  // 3136
#define FC1_OUT 128
#define FC2_IN 128
#define FC2_OUT 10

// 函数声明
uint32_t read_big_endian(FILE *fp);
uint8_t** read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols);
uint8_t* read_mnist_labels(const char *filename, uint32_t *num_labels);
void readbmp(const char* filename, float* img);
float max_of_four(float a, float b, float c, float d);

// CNN前向传播函数
void cnn_forward(
    float *input,             // 输入图像 [1, 28, 28]
    
    // 卷积层1参数
    float *conv1_weight,      // [32, 1, 3, 3]
    float *conv1_bias,        // [32]
    
    // 卷积层2参数
    float *conv2_weight,      // [64, 32, 3, 3]
    float *conv2_bias,        // [64]
    
    // 全连接层1参数
    float *fc1_weight,        // [128, 3136]
    float *fc1_bias,          // [128]
    
    // 全连接层2参数
    float *fc2_weight,        // [10, 128]
    float *fc2_bias,          // [10]
    
    // 输出
    float *output             // [10]
);

// ReLU激活函数
void relu(float *x, int len) {
    for (int i = 0; i < len; i++) {
        if (x[i] < 0) x[i] = 0;
    }
}

// 2x2最大池化
void max_pool_2x2(float *input, int in_h, int in_w, int channels, float *output) {
    int out_h = in_h / 2;
    int out_w = in_w / 2;
    
    for (int c = 0; c < channels; c++) {
        for (int h = 0; h < out_h; h++) {
            for (int w = 0; w < out_w; w++) {
                int in_idx_base = c * in_h * in_w + 2 * h * in_w + 2 * w;
                float max_val = input[in_idx_base];
                max_val = fmaxf(max_val, input[in_idx_base + 1]);
                max_val = fmaxf(max_val, input[in_idx_base + in_w]);
                max_val = fmaxf(max_val, input[in_idx_base + in_w + 1]);
                
                output[c * out_h * out_w + h * out_w + w] = max_val;
            }
        }
    }
}

// 2D卷积操作 (带padding)
void conv2d(float *input, int in_h, int in_w, int in_c,
            float *weight, float *bias, int out_c, int kernel_size,
            int padding, float *output, int out_h, int out_w) {
    
    // 创建带padding的临时输入
    int padded_h = in_h + 2 * padding;
    int padded_w = in_w + 2 * padding;
    float *padded_input = (float *)calloc(in_c * padded_h * padded_w, sizeof(float));
    
    // 填充输入
    for (int c = 0; c < in_c; c++) {
        for (int h = 0; h < in_h; h++) {
            for (int w = 0; w < in_w; w++) {
                padded_input[c * padded_h * padded_w + (h + padding) * padded_w + (w + padding)] = 
                    input[c * in_h * in_w + h * in_w + w];
            }
        }
    }
    
    // 进行卷积计算
    for (int oc = 0; oc < out_c; oc++) {
        for (int oh = 0; oh < out_h; oh++) {
            for (int ow = 0; ow < out_w; ow++) {
                float sum = 0.0f;
                
                for (int ic = 0; ic < in_c; ic++) {
                    for (int kh = 0; kh < kernel_size; kh++) {
                        for (int kw = 0; kw < kernel_size; kw++) {
                            int h_idx = oh + kh;
                            int w_idx = ow + kw;
                            float in_val = padded_input[ic * padded_h * padded_w + h_idx * padded_w + w_idx];
                            float weight_val = weight[oc * in_c * kernel_size * kernel_size + 
                                                   ic * kernel_size * kernel_size + 
                                                   kh * kernel_size + kw];
                            sum += in_val * weight_val;
                        }
                    }
                }
                
                // 添加偏置并存储到输出
                output[oc * out_h * out_w + oh * out_w + ow] = sum + bias[oc];
            }
        }
    }
    
    free(padded_input);
}

// 全连接层运算
void fully_connected(float *input, float *weight, float *bias, 
                     int in_features, int out_features, float *output) {
    for (int i = 0; i < out_features; i++) {
        float sum = 0.0f;
        for (int j = 0; j < in_features; j++) {
            sum += input[j] * weight[i * in_features + j];
        }
        output[i] = sum + bias[i];
    }
}

int main(int argc, char* argv[]) {
    assert(sizeof(float) == 4); // 确保float是4字节
    
    // 打开参数文件
    FILE *fp = fopen("../parameters_cnn.bin", "rb");
    if (!fp) {
        printf("无法打开参数文件！\n");
        return 1;
    }
    
    // 分配内存
    float *conv1_weight = (float *)malloc(CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE * sizeof(float));
    float *conv1_bias = (float *)malloc(CONV1_OUT_CHANNELS * sizeof(float));
    float *conv2_weight = (float *)malloc(CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE * sizeof(float));
    float *conv2_bias = (float *)malloc(CONV2_OUT_CHANNELS * sizeof(float));
    float *fc1_weight = (float *)malloc(FC1_OUT * FC1_IN * sizeof(float));
    float *fc1_bias = (float *)malloc(FC1_OUT * sizeof(float));
    float *fc2_weight = (float *)malloc(FC2_OUT * FC2_IN * sizeof(float));
    float *fc2_bias = (float *)malloc(FC2_OUT * sizeof(float));
    
    if (!conv1_weight || !conv1_bias || !conv2_weight || !conv2_bias || 
        !fc1_weight || !fc1_bias || !fc2_weight || !fc2_bias) {
        printf("内存分配失败！\n");
        fclose(fp);
        return 2;
    }
    
    // 读取参数
    size_t nread;
    
    // 读取卷积层1参数
    nread = fread(conv1_weight, sizeof(float), CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE, fp);
    if (nread != CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE) {
        printf("conv1_weight读取失败, read %zu elements\n", nread);
        return 3;
    }
    
    nread = fread(conv1_bias, sizeof(float), CONV1_OUT_CHANNELS, fp);
    if (nread != CONV1_OUT_CHANNELS) {
        printf("conv1_bias读取失败\n");
        return 4;
    }
    
    // 读取卷积层2参数
    nread = fread(conv2_weight, sizeof(float), CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE, fp);
    if (nread != CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE) {
        printf("conv2_weight读取失败\n");
        return 5;
    }
    
    nread = fread(conv2_bias, sizeof(float), CONV2_OUT_CHANNELS, fp);
    if (nread != CONV2_OUT_CHANNELS) {
        printf("conv2_bias读取失败\n");
        return 6;
    }
    
    // 读取全连接层1参数
    nread = fread(fc1_weight, sizeof(float), FC1_OUT * FC1_IN, fp);
    if (nread != FC1_OUT * FC1_IN) {
        printf("fc1_weight读取失败\n");
        return 7;
    }
    
    nread = fread(fc1_bias, sizeof(float), FC1_OUT, fp);
    if (nread != FC1_OUT) {
        printf("fc1_bias读取失败\n");
        return 8;
    }
    
    // 读取全连接层2参数
    nread = fread(fc2_weight, sizeof(float), FC2_OUT * FC2_IN, fp);
    if (nread != FC2_OUT * FC2_IN) {
        printf("fc2_weight读取失败\n");
        return 9;
    }
    
    nread = fread(fc2_bias, sizeof(float), FC2_OUT, fp);
    if (nread != FC2_OUT) {
        printf("fc2_bias读取失败\n");
        return 10;
    }
    
    fclose(fp);
    
    if (argc == 2) {
        // 单个图像推理模式
        float img[IMG_C * IMG_H * IMG_W];
        float output[10];
        
        // 读取BMP图像
        readbmp(argv[1], img);
        
        // 执行CNN前向推理
        cnn_forward(img, conv1_weight, conv1_bias, conv2_weight, conv2_bias,
                   fc1_weight, fc1_bias, fc2_weight, fc2_bias, output);
        
        // 找出最大值的索引
        int predicted = 0;
        for (int i = 1; i < 10; i++) {
            if (output[predicted] < output[i]) {
                predicted = i;
            }
        }
        
        printf("识别结果: %d\n", predicted);
    } else {
        // 测试集评估模式
        const char *image_file = "../data/t10k-images-idx3-ubyte";
        const char *label_file = "../data/t10k-labels-idx1-ubyte";
        
        uint32_t num_images, rows, cols, num_labels;
        uint8_t **images = read_mnist_images(image_file, &num_images, &rows, &cols);
        uint8_t *labels = read_mnist_labels(label_file, &num_labels);
        
        printf("测试集图像数量: %u\n", num_images);
        
        // 统计正确预测数量
        int correct = 0;
        for (int i = 0; i < num_images; i++) {
            float img[IMG_C * IMG_H * IMG_W];
            float output[10];
            
            // 将图像数据转换为浮点数
            for (int j = 0; j < IMG_H * IMG_W; j++) {
                img[j] = images[i][j] / 255.0f;
            }
            
            // 执行CNN前向推理
            cnn_forward(img, conv1_weight, conv1_bias, conv2_weight, conv2_bias,
                       fc1_weight, fc1_bias, fc2_weight, fc2_bias, output);
            
            // 找出最大值的索引
            int predicted = 0;
            for (int j = 1; j < 10; j++) {
                if (output[predicted] < output[j]) {
                    predicted = j;
                }
            }
            
            // 检查预测是否正确
            if (predicted == labels[i]) {
                correct++;
            }
            
            // 每处理1000张图像打印一次进度
            if ((i + 1) % 1000 == 0) {
                printf("已处理 %d/%u 张图像\n", i + 1, num_images);
            }
        }
        
        // 打印准确率
        float accuracy = (float)correct / num_images;
        printf("测试准确率: %.4f (%d/%u)\n", accuracy, correct, num_images);
        
        // 释放测试集内存
        for (uint32_t i = 0; i < num_images; i++) {
            free(images[i]);
        }
        free(images);
        free(labels);
    }
    
    // 释放参数内存
    free(conv1_weight);
    free(conv1_bias);
    free(conv2_weight);
    free(conv2_bias);
    free(fc1_weight);
    free(fc1_bias);
    free(fc2_weight);
    free(fc2_bias);
    
    return 0;
}

// CNN前向传播函数实现
void cnn_forward(
    float *input,             // 输入图像 [1, 28, 28]
    float *conv1_weight,      // [32, 1, 3, 3]
    float *conv1_bias,        // [32]
    float *conv2_weight,      // [64, 32, 3, 3]
    float *conv2_bias,        // [64]
    float *fc1_weight,        // [128, 3136]
    float *fc1_bias,          // [128]
    float *fc2_weight,        // [10, 128]
    float *fc2_bias,          // [10]
    float *output             // [10]
) {
    // 分配临时缓冲区
    float *conv1_output = (float *)malloc(CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W * sizeof(float));
    float *conv1_relu_output = (float *)malloc(CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W * sizeof(float));
    float *pool1_output = (float *)malloc(CONV1_OUT_CHANNELS * POOL1_OUT_H * POOL1_OUT_W * sizeof(float));
    
    float *conv2_output = (float *)malloc(CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W * sizeof(float));
    float *conv2_relu_output = (float *)malloc(CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W * sizeof(float));
    float *pool2_output = (float *)malloc(CONV2_OUT_CHANNELS * POOL2_OUT_H * POOL2_OUT_W * sizeof(float));
    
    float *flatten_output = (float *)malloc(FC1_IN * sizeof(float));
    float *fc1_output = (float *)malloc(FC1_OUT * sizeof(float));
    float *fc1_relu_output = (float *)malloc(FC1_OUT * sizeof(float));
    
    // 1. 第一个卷积层
    conv2d(input, IMG_H, IMG_W, IMG_C, 
           conv1_weight, conv1_bias, CONV1_OUT_CHANNELS, CONV1_KERNEL_SIZE, 
           CONV1_PADDING, conv1_output, CONV1_OUT_H, CONV1_OUT_W);
    
    // 2. ReLU激活
    memcpy(conv1_relu_output, conv1_output, CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W * sizeof(float));
    relu(conv1_relu_output, CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W);
    
    // 3. 第一次池化
    max_pool_2x2(conv1_relu_output, CONV1_OUT_H, CONV1_OUT_W, CONV1_OUT_CHANNELS, pool1_output);
    
    // 4. 第二个卷积层
    conv2d(pool1_output, POOL1_OUT_H, POOL1_OUT_W, CONV1_OUT_CHANNELS,
           conv2_weight, conv2_bias, CONV2_OUT_CHANNELS, CONV2_KERNEL_SIZE,
           CONV2_PADDING, conv2_output, CONV2_OUT_H, CONV2_OUT_W);
    
    // 5. ReLU激活
    memcpy(conv2_relu_output, conv2_output, CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W * sizeof(float));
    relu(conv2_relu_output, CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W);
    
    // 6. 第二次池化
    max_pool_2x2(conv2_relu_output, CONV2_OUT_H, CONV2_OUT_W, CONV2_OUT_CHANNELS, pool2_output);
    
    // 7. 展平操作 (64 * 7 * 7 = 3136)
    memcpy(flatten_output, pool2_output, FC1_IN * sizeof(float));
    
    // 8. 第一个全连接层
    fully_connected(flatten_output, fc1_weight, fc1_bias, FC1_IN, FC1_OUT, fc1_output);
    
    // 9. ReLU激活
    memcpy(fc1_relu_output, fc1_output, FC1_OUT * sizeof(float));
    relu(fc1_relu_output, FC1_OUT);
    
    // 10. 第二个全连接层 (输出层)
    fully_connected(fc1_relu_output, fc2_weight, fc2_bias, FC2_IN, FC2_OUT, output);
    
    // 释放临时缓冲区
    free(conv1_output);
    free(conv1_relu_output);
    free(pool1_output);
    free(conv2_output);
    free(conv2_relu_output);
    free(pool2_output);
    free(flatten_output);
    free(fc1_output);
    free(fc1_relu_output);
}

void readbmp(const char* filename, float* img)
{
    FILE *f = fopen(filename, "rb");
    if (!f) {
        printf("Can not open file %s\n", filename);
        return;
    }

    // 读取文件头（14字节）
    uint8_t file_header[14];
    fread(file_header, 1, 14, f);

    // 读取信息头（40字节）
    uint8_t info_header[40];
    fread(info_header, 1, 40, f);

    int width  = *(int*)&info_header[4];
    int height = *(int*)&info_header[8];
    uint16_t bit_count = *(uint16_t*)&info_header[14];
    if (bit_count != 24) {
        printf("Only 24-bit BMP is supported.\n");
        fclose(f);
        return;
    }

    // 获取像素数据偏移
    uint32_t pixel_offset = *(uint32_t*)&file_header[10];
    fseek(f, pixel_offset, SEEK_SET);

    // 每行字节数4字节对齐
    int row_bytes = ((width * 3 + 3) / 4) * 4;
    uint8_t *row_buf = malloc(row_bytes);

    // BMP像素自下而上存储
    for (int y = height - 1; y >= 0; --y) {
        fread(row_buf, 1, row_bytes, f);
        for (int x = 0; x < width; ++x) {
            uint8_t B = row_buf[x * 3 + 0];
            uint8_t G = row_buf[x * 3 + 1];
            uint8_t R = row_buf[x * 3 + 2];
            float gray = (0.299f * R + 0.587f * G + 0.114f * B) / 255.0f;
            img[y * width + x] = gray;
        }
    }

    free(row_buf);
    fclose(f);
}

// 读取大端整数（4字节）
uint32_t read_big_endian(FILE *fp) {
    uint8_t bytes[4];
    fread(bytes, sizeof(uint8_t), 4, fp);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// 读取图像数据
uint8_t** read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open image file");
        exit(1);
    }

    uint32_t magic = read_big_endian(fp);
    if (magic != 0x00000803) {
        fprintf(stderr, "Invalid image file magic number: %08X\n", magic);
        exit(1);
    }

    *num_images = read_big_endian(fp);
    *rows = read_big_endian(fp);
    *cols = read_big_endian(fp);

    uint8_t **images = malloc(*num_images * sizeof(uint8_t*));
    for (uint32_t i = 0; i < *num_images; ++i) {
        images[i] = malloc((*rows) * (*cols) * sizeof(uint8_t));
        fread(images[i], sizeof(uint8_t), (*rows) * (*cols), fp);
    }

    fclose(fp);
    return images;
}

// 读取标签数据
uint8_t* read_mnist_labels(const char *filename, uint32_t *num_labels) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open label file");
        exit(1);
    }

    uint32_t magic = read_big_endian(fp);
    if (magic != 0x00000801) {
        fprintf(stderr, "Invalid label file magic number: %08X\n", magic);
        exit(1);
    }

    *num_labels = read_big_endian(fp);
    uint8_t *labels = malloc(*num_labels * sizeof(uint8_t));
    fread(labels, sizeof(uint8_t), *num_labels, fp);

    fclose(fp);
    return labels;
}
