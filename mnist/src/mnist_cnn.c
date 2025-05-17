#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

// 网络参数维度常量
#define IMG_H 28
#define IMG_W 28
#define CONV1_IN_CHANNELS 1
#define CONV1_OUT_CHANNELS 32
#define CONV1_KERNEL_SIZE 3
#define CONV1_PADDING 1

#define CONV2_IN_CHANNELS 32
#define CONV2_OUT_CHANNELS 64
#define CONV2_KERNEL_SIZE 3
#define CONV2_PADDING 1

#define POOL_SIZE 2
#define POOL_STRIDE 2

#define FC1_IN 64 * 7 * 7  // 经过两次池化后，特征图尺寸为 7x7
#define FC1_OUT 128
#define FC2_IN 128
#define FC2_OUT 10

// 函数声明
uint32_t read_big_endian(FILE *fp);
uint8_t** read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols);
uint8_t* read_mnist_labels(const char *filename, uint32_t *num_labels);
void readbmp(const char* filename, float* img);
float max_of_four(float a, float b, float c, float d);
void relu(float* data, int size);

// CNN正向推理函数
void mnist_cnn_forward(
    float *x,
    float *conv1_weight, float *conv1_bias,
    float *conv2_weight, float *conv2_bias,
    float *fc1_weight, float *fc1_bias,
    float *fc2_weight, float *fc2_bias,
    float *output
);

int main(int argc, char* argv[])
{
    assert(sizeof(float) == 4); // 确保float为4字节

    FILE *fp = fopen("../ExportPara/parameters_cnn.bin", "rb");
    if (!fp)
    {
        printf("无法打开参数文件！\n");
        return 1;
    }

    // 分配内存
    float *conv1_weight = (float *)malloc(sizeof(float) * CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE);
    float *conv1_bias = (float *)malloc(sizeof(float) * CONV1_OUT_CHANNELS);
    float *conv2_weight = (float *)malloc(sizeof(float) * CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE);
    float *conv2_bias = (float *)malloc(sizeof(float) * CONV2_OUT_CHANNELS);
    float *fc1_weight = (float *)malloc(sizeof(float) * FC1_IN * FC1_OUT);
    float *fc1_bias = (float *)malloc(sizeof(float) * FC1_OUT);
    float *fc2_weight = (float *)malloc(sizeof(float) * FC2_IN * FC2_OUT);
    float *fc2_bias = (float *)malloc(sizeof(float) * FC2_OUT);

    if (!conv1_weight || !conv1_bias || !conv2_weight || !conv2_bias || 
        !fc1_weight || !fc1_bias || !fc2_weight || !fc2_bias)
    {
        printf("内存分配失败！\n");
        fclose(fp);
        return 2;
    }

    // 读取参数
    size_t nread;
    
    nread = fread(conv1_weight, sizeof(float), CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE, fp);
    if (nread != CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE)
    {
        printf("conv1_weight读取失败\n");
        return 3;
    }
    
    nread = fread(conv1_bias, sizeof(float), CONV1_OUT_CHANNELS, fp);
    if (nread != CONV1_OUT_CHANNELS)
    {
        printf("conv1_bias读取失败\n");
        return 4;
    }
    
    nread = fread(conv2_weight, sizeof(float), CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE, fp);
    if (nread != CONV2_OUT_CHANNELS * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE)
    {
        printf("conv2_weight读取失败\n");
        return 5;
    }
    
    nread = fread(conv2_bias, sizeof(float), CONV2_OUT_CHANNELS, fp);
    if (nread != CONV2_OUT_CHANNELS)
    {
        printf("conv2_bias读取失败\n");
        return 6;
    }
    
    nread = fread(fc1_weight, sizeof(float), FC1_IN * FC1_OUT, fp);
    if (nread != FC1_IN * FC1_OUT)
    {
        printf("fc1_weight读取失败\n");
        return 7;
    }
    
    nread = fread(fc1_bias, sizeof(float), FC1_OUT, fp);
    if (nread != FC1_OUT)
    {
        printf("fc1_bias读取失败\n");
        return 8;
    }
    
    nread = fread(fc2_weight, sizeof(float), FC2_IN * FC2_OUT, fp);
    if (nread != FC2_IN * FC2_OUT)
    {
        printf("fc2_weight读取失败\n");
        return 9;
    }
    
    nread = fread(fc2_bias, sizeof(float), FC2_OUT, fp);
    if (nread != FC2_OUT)
    {
        printf("fc2_bias读取失败\n");
        return 10;
    }

    fclose(fp);

    if (argc == 2)
    {
        // 单个BMP图像的推理
        float img[IMG_H * IMG_W];
        float output[10]; // 输出结果

        readbmp(argv[1], img);

        // 执行推理
        mnist_cnn_forward(img, 
                         conv1_weight, conv1_bias,
                         conv2_weight, conv2_bias,
                         fc1_weight, fc1_bias,
                         fc2_weight, fc2_bias,
                         output);

        // 找出最大概率的数字
        int predicted = 0;
        for (int i = 0; i < 10; ++i)
        {
            if (output[predicted] < output[i])
                predicted = i;
        }

        printf("识别结果：%d\n", predicted);
    }
    else
    {
        // 测试集准确率评估
        const char *image_file = "../data/t10k-images-idx3-ubyte";
        const char *label_file = "../data/t10k-labels-idx1-ubyte";

        uint32_t num_images, rows, cols, num_labels;
        uint8_t **images = read_mnist_images(image_file, &num_images, &rows, &cols);
        uint8_t *labels = read_mnist_labels(label_file, &num_labels);

        int correct = 0;
        for (int i = 0; i < num_images; ++i) {
            float img[IMG_H * IMG_W];
            float output[10]; // 输出结果
            
            // 将图像数据转换为浮点数
            for (int j = 0; j < IMG_H * IMG_W; ++j)
            {
                img[j] = images[i][j] / 255.0f;
            }

            // 执行推理
            mnist_cnn_forward(img, 
                             conv1_weight, conv1_bias,
                             conv2_weight, conv2_bias,
                             fc1_weight, fc1_bias,
                             fc2_weight, fc2_bias,
                             output);

            // 找出最大概率的数字
            int predicted = 0;
            for (int k = 0; k < 10; ++k)
            {
                if (output[predicted] < output[k])
                    predicted = k;
            }
            
            if (predicted == labels[i])
                correct++;
        }
        
        printf("测试集准确率：%f\n", correct * 1.0 / num_images);
        
        // 释放内存
        for (int i = 0; i < num_images; ++i) {
            free(images[i]);
        }
        free(images);
        free(labels);
    }

    // 释放模型参数内存
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

// CNN正向推理实现
void mnist_cnn_forward(
    float *x,
    float *conv1_weight, float *conv1_bias,
    float *conv2_weight, float *conv2_bias,
    float *fc1_weight, float *fc1_bias,
    float *fc2_weight, float *fc2_bias,
    float *output
)
{
    const int padded_h = IMG_H + 2 * CONV1_PADDING;
    const int padded_w = IMG_W + 2 * CONV1_PADDING;
    
    // 1. 第一层卷积 (CONV1)
    float *padded_input = (float *)calloc(padded_h * padded_w, sizeof(float));
    float *conv1_output = (float *)malloc(CONV1_OUT_CHANNELS * IMG_H * IMG_W * sizeof(float));
    
    // 填充输入
    for (int i = 0; i < IMG_H; i++) {
        for (int j = 0; j < IMG_W; j++) {
            padded_input[(i + CONV1_PADDING) * padded_w + (j + CONV1_PADDING)] = x[i * IMG_W + j];
        }
    }
    
    // 第一层卷积
    for (int oc = 0; oc < CONV1_OUT_CHANNELS; oc++) {
        for (int h = 0; h < IMG_H; h++) {
            for (int w = 0; w < IMG_W; w++) {
                float sum = conv1_bias[oc];
                for (int ic = 0; ic < CONV1_IN_CHANNELS; ic++) {
                    for (int kh = 0; kh < CONV1_KERNEL_SIZE; kh++) {
                        for (int kw = 0; kw < CONV1_KERNEL_SIZE; kw++) {
                            int input_h = h + kh;
                            int input_w = w + kw;
                            float input_val = padded_input[input_h * padded_w + input_w];
                            int weight_idx = ((oc * CONV1_IN_CHANNELS + ic) * CONV1_KERNEL_SIZE + kh) * CONV1_KERNEL_SIZE + kw;
                            sum += input_val * conv1_weight[weight_idx];
                        }
                    }
                }
                conv1_output[(oc * IMG_H + h) * IMG_W + w] = sum;
            }
        }
    }
    
    free(padded_input);
    
    // 应用ReLU激活函数
    relu(conv1_output, CONV1_OUT_CHANNELS * IMG_H * IMG_W);
    
    // 2. 第二层卷积 (CONV2)
    const int padded_h2 = IMG_H + 2 * CONV2_PADDING;
    const int padded_w2 = IMG_W + 2 * CONV2_PADDING;
    
    float *padded_conv1_output = (float *)calloc(CONV1_OUT_CHANNELS * padded_h2 * padded_w2, sizeof(float));
    float *conv2_output = (float *)malloc(CONV2_OUT_CHANNELS * IMG_H * IMG_W * sizeof(float));
    
    // 填充CONV1输出
    for (int c = 0; c < CONV1_OUT_CHANNELS; c++) {
        for (int i = 0; i < IMG_H; i++) {
            for (int j = 0; j < IMG_W; j++) {
                padded_conv1_output[(c * padded_h2 + (i + CONV2_PADDING)) * padded_w2 + (j + CONV2_PADDING)] = 
                    conv1_output[(c * IMG_H + i) * IMG_W + j];
            }
        }
    }
    
    free(conv1_output);
    
    // 第二层卷积
    for (int oc = 0; oc < CONV2_OUT_CHANNELS; oc++) {
        for (int h = 0; h < IMG_H; h++) {
            for (int w = 0; w < IMG_W; w++) {
                float sum = conv2_bias[oc];
                for (int ic = 0; ic < CONV2_IN_CHANNELS; ic++) {
                    for (int kh = 0; kh < CONV2_KERNEL_SIZE; kh++) {
                        for (int kw = 0; kw < CONV2_KERNEL_SIZE; kw++) {
                            int input_h = h + kh;
                            int input_w = w + kw;
                            float input_val = padded_conv1_output[(ic * padded_h2 + input_h) * padded_w2 + input_w];
                            int weight_idx = ((oc * CONV2_IN_CHANNELS + ic) * CONV2_KERNEL_SIZE + kh) * CONV2_KERNEL_SIZE + kw;
                            sum += input_val * conv2_weight[weight_idx];
                        }
                    }
                }
                conv2_output[(oc * IMG_H + h) * IMG_W + w] = sum;
            }
        }
    }
    
    free(padded_conv1_output);
    
    // 应用ReLU激活函数
    relu(conv2_output, CONV2_OUT_CHANNELS * IMG_H * IMG_W);
    
    // 3. 第一次池化 (POOL1)
    const int pool1_h = IMG_H / POOL_SIZE;
    const int pool1_w = IMG_W / POOL_SIZE;
    float *pool1_output = (float *)malloc(CONV2_OUT_CHANNELS * pool1_h * pool1_w * sizeof(float));
    
    for (int c = 0; c < CONV2_OUT_CHANNELS; c++) {
        for (int h = 0; h < pool1_h; h++) {
            for (int w = 0; w < pool1_w; w++) {
                float max_val = -INFINITY;
                for (int ph = 0; ph < POOL_SIZE; ph++) {
                    for (int pw = 0; pw < POOL_SIZE; pw++) {
                        int input_h = h * POOL_SIZE + ph;
                        int input_w = w * POOL_SIZE + pw;
                        float val = conv2_output[(c * IMG_H + input_h) * IMG_W + input_w];
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
                pool1_output[(c * pool1_h + h) * pool1_w + w] = max_val;
            }
        }
    }
    
    free(conv2_output);
    
    // 4. 第二次池化 (POOL2)
    const int pool2_h = pool1_h / POOL_SIZE;
    const int pool2_w = pool1_w / POOL_SIZE;
    float *pool2_output = (float *)malloc(CONV2_OUT_CHANNELS * pool2_h * pool2_w * sizeof(float));
    
    for (int c = 0; c < CONV2_OUT_CHANNELS; c++) {
        for (int h = 0; h < pool2_h; h++) {
            for (int w = 0; w < pool2_w; w++) {
                float max_val = -INFINITY;
                for (int ph = 0; ph < POOL_SIZE; ph++) {
                    for (int pw = 0; pw < POOL_SIZE; pw++) {
                        int input_h = h * POOL_SIZE + ph;
                        int input_w = w * POOL_SIZE + pw;
                        float val = pool1_output[(c * pool1_h + input_h) * pool1_w + input_w];
                        if (val > max_val) {
                            max_val = val;
                        }
                    }
                }
                pool2_output[(c * pool2_h + h) * pool2_w + w] = max_val;
            }
        }
    }
    
    free(pool1_output);
    
    // 5. 全连接层1 (FC1)
    float *fc1_output = (float *)malloc(FC1_OUT * sizeof(float));
    
    for (int i = 0; i < FC1_OUT; i++) {
        float sum = fc1_bias[i];
        for (int j = 0; j < FC1_IN; j++) {
            sum += pool2_output[j] * fc1_weight[i * FC1_IN + j];
        }
        fc1_output[i] = sum;
    }
    
    free(pool2_output);
    
    // 应用ReLU激活函数
    relu(fc1_output, FC1_OUT);
    
    // 6. 全连接层2 (FC2)
    for (int i = 0; i < FC2_OUT; i++) {
        float sum = fc2_bias[i];
        for (int j = 0; j < FC2_IN; j++) {
            sum += fc1_output[j] * fc2_weight[i * FC2_IN + j];
        }
        output[i] = sum;
    }
    
    free(fc1_output);
}

// ReLU激活函数
void relu(float* data, int size) {
    for (int i = 0; i < size; i++) {
        if (data[i] < 0) {
            data[i] = 0;
        }
    }
}

// 读取BMP图像
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
