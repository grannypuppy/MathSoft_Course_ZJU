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
#define CONV1_OUT_CHANNELS 2
#define CONV1_KERNEL_SIZE 3
#define CONV1_PADDING 1
#define CONV1_OUT_W 28
#define CONV1_OUT_H 28

// 卷积层2参数
#define CONV2_IN_CHANNELS 2
#define CONV2_OUT_CHANNELS 3
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
#define FC1_IN 3 * 7 * 7 // 147
#define FC1_OUT 6
#define FC2_IN 6
#define FC2_OUT 10

// 函数声明
uint32_t read_big_endian(FILE *fp);
uint8_t **read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols);
uint8_t *read_mnist_labels(const char *filename, uint32_t *num_labels);
void readbmp(const char *filename, float *img);
void cnn_forward(float *input, float *conv1_weight, float *conv1_bias, 
                float *conv2_weight, float *conv2_bias, 
                float *fc1_weight, float *fc1_bias, 
                float *fc2_weight, float *fc2_bias, 
                float *output);

int main(int argc, char *argv[])
{
    assert(sizeof(float) == 4); // 确保float是4字节

    // 打开参数文件
    FILE *fp = fopen("./ExportPara/parameters_cnn.bin", "rb");
    if (!fp)
    {
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
        !fc1_weight || !fc1_bias || !fc2_weight || !fc2_bias)
    {
        printf("内存分配失败！\n");
        fclose(fp);
        return 2;
    }

    // 读取参数
    size_t nread;

    // 读取卷积层1参数
    nread = fread(conv1_weight, sizeof(float), CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE, fp);
    if (nread != CONV1_OUT_CHANNELS * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE)
    {
        printf("conv1_weight读取失败, read %zu elements\n", nread);
        return 3;
    }

    nread = fread(conv1_bias, sizeof(float), CONV1_OUT_CHANNELS, fp);
    if (nread != CONV1_OUT_CHANNELS)
    {
        printf("conv1_bias读取失败\n");
        return 4;
    }

    // 读取卷积层2参数
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

    // 读取全连接层1参数
    nread = fread(fc1_weight, sizeof(float), FC1_OUT * FC1_IN, fp);
    if (nread != FC1_OUT * FC1_IN)
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

    // 读取全连接层2参数
    nread = fread(fc2_weight, sizeof(float), FC2_OUT * FC2_IN, fp);
    if (nread != FC2_OUT * FC2_IN)
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
        for (int i = 0; i < 10; i++)
        {
            //printf("%f ", output[i]);
            if (output[predicted] < output[i])
            {
                predicted = i;
            }
        }

        printf("识别结果: %d\n", predicted);
    }
    else
    {
        // 测试集评估模式
        const char *image_file = "./data/t10k-images-idx3-ubyte";
        const char *label_file = "./data/t10k-labels-idx1-ubyte";

        uint32_t num_images, rows, cols, num_labels;
        uint8_t **images = read_mnist_images(image_file, &num_images, &rows, &cols);
        uint8_t *labels = read_mnist_labels(label_file, &num_labels);

        printf("测试集图像数量: %u\n", num_images);

        // 统计正确预测数量
        int correct = 0;
        for (int i = 0; i < num_images; i++)
        {
            float img[IMG_C * IMG_H * IMG_W];
            float output[10];

            // 将图像数据转换为浮点数
            for (int j = 0; j < IMG_H * IMG_W; j++)
            {
                img[j] = images[i][j] / 255.0f; // 简单归一化，cnn_forward中会做正确的标准化
            }

            // 执行CNN前向推理
            cnn_forward(img, conv1_weight, conv1_bias, conv2_weight, conv2_bias,
                        fc1_weight, fc1_bias, fc2_weight, fc2_bias, output);

            // 找出最大值的索引
            int predicted = 0;
            for (int j = 1; j < 10; j++)
            {
                if (output[predicted] < output[j])
                {
                    predicted = j;
                }
            }

            // 检查预测是否正确
            if (predicted == labels[i])
            {
                correct++;
            }

            // 每处理1000张图像打印一次进度
            if ((i + 1) % 1000 == 0)
            {
                printf("已处理 %d/%u 张图像\n", i + 1, num_images);
            }
        }

        // 打印准确率
        float accuracy = (float)correct / num_images;
        printf("测试准确率: %.4f (%d/%u)\n", accuracy, correct, num_images);

        // 释放测试集内存
        for (uint32_t i = 0; i < num_images; i++)
        {
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

// 简化的CNN前向传播函数实现
void cnn_forward(
    float *input,        // 输入图像 [1, 28, 28]
    float *conv1_weight, // [2, 1, 3, 3]
    float *conv1_bias,   // [2]
    float *conv2_weight, // [3, 2, 3, 3]
    float *conv2_bias,   // [3]
    float *fc1_weight,   // [16, 147]
    float *fc1_bias,     // [16]
    float *fc2_weight,   // [10, 16]
    float *fc2_bias,     // [10]
    float *output        // [10]
)
{
    // 临时缓冲区
    float conv1_output[CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W];
    float relu1_output[CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W];
    float conv2_output[CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W];
    float relu2_output[CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W];
    float pool1_output[CONV2_OUT_CHANNELS * POOL1_OUT_H * POOL1_OUT_W];
    float pool2_output[CONV2_OUT_CHANNELS * POOL2_OUT_H * POOL2_OUT_W];
    float fc1_output[FC1_OUT];
    
    // 输入标准化: 转换为与PyTorch相同的标准化格式
    float normalized_input[IMG_C * IMG_H * IMG_W];
    for (int i = 0; i < IMG_H * IMG_W; i++) {
        // 应用与PyTorch相同的标准化: (x - mean) / std
        normalized_input[i] = (input[i] - 0.1307f) / 0.3081f;
    }
    
    // 1. 第一个卷积层: input -> conv1_output
    for (int oc = 0; oc < CONV1_OUT_CHANNELS; oc++) {
        for (int oh = 0; oh < CONV1_OUT_H; oh++) {
            for (int ow = 0; ow < CONV1_OUT_W; ow++) {
                float sum = 0.0f;
                
                for (int kh = 0; kh < CONV1_KERNEL_SIZE; kh++) {
                    for (int kw = 0; kw < CONV1_KERNEL_SIZE; kw++) {
                        int h_idx = oh + kh - CONV1_PADDING;
                        int w_idx = ow + kw - CONV1_PADDING;
                        
                        if (h_idx >= 0 && h_idx < IMG_H && w_idx >= 0 && w_idx < IMG_W) {
                            float in_val = normalized_input[h_idx * IMG_W + w_idx];
                            float weight_val = conv1_weight[oc * CONV1_IN_CHANNELS * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE + 
                                                          0 * CONV1_KERNEL_SIZE * CONV1_KERNEL_SIZE + 
                                                          kh * CONV1_KERNEL_SIZE + kw];
                            sum += in_val * weight_val;
                        }
                    }
                }
                
                // 添加偏置
                sum += conv1_bias[oc];
                conv1_output[oc * CONV1_OUT_H * CONV1_OUT_W + oh * CONV1_OUT_W + ow] = sum;
            }
        }
    }
    
    // 应用ReLU到conv1_output
    for (int i = 0; i < CONV1_OUT_CHANNELS * CONV1_OUT_H * CONV1_OUT_W; i++) {
        relu1_output[i] = conv1_output[i] > 0 ? conv1_output[i] : 0;
    }
    
    // 2. 第二个卷积层: relu1_output -> conv2_output
    for (int oc = 0; oc < CONV2_OUT_CHANNELS; oc++) {
        for (int oh = 0; oh < CONV2_OUT_H; oh++) {
            for (int ow = 0; ow < CONV2_OUT_W; ow++) {
                float sum = 0.0f;
                
                for (int ic = 0; ic < CONV2_IN_CHANNELS; ic++) {
                    for (int kh = 0; kh < CONV2_KERNEL_SIZE; kh++) {
                        for (int kw = 0; kw < CONV2_KERNEL_SIZE; kw++) {
                            int h_idx = oh + kh - CONV2_PADDING;
                            int w_idx = ow + kw - CONV2_PADDING;
                            
                            if (h_idx >= 0 && h_idx < CONV1_OUT_H && w_idx >= 0 && w_idx < CONV1_OUT_W) {
                                float in_val = relu1_output[ic * CONV1_OUT_H * CONV1_OUT_W + h_idx * CONV1_OUT_W + w_idx];
                                float weight_val = conv2_weight[oc * CONV2_IN_CHANNELS * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE + 
                                                              ic * CONV2_KERNEL_SIZE * CONV2_KERNEL_SIZE +
                                                              kh * CONV2_KERNEL_SIZE + kw];
                                sum += in_val * weight_val;
                            }
                        }
                    }
                }
                
                // 添加偏置
                sum += conv2_bias[oc];
                conv2_output[oc * CONV2_OUT_H * CONV2_OUT_W + oh * CONV2_OUT_W + ow] = sum;
            }
        }
    }

    // 应用ReLU到conv2_output
    for (int i = 0; i < CONV2_OUT_CHANNELS * CONV2_OUT_H * CONV2_OUT_W; i++) {
        relu2_output[i] = conv2_output[i] > 0 ? conv2_output[i] : 0;
    }
    
    // 3. 第一次池化: relu2_output -> pool1_output
    for (int c = 0; c < CONV2_OUT_CHANNELS; c++) {
        for (int h = 0; h < POOL1_OUT_H; h++) {
            for (int w = 0; w < POOL1_OUT_W; w++) {
                float max_val = relu2_output[c * CONV2_OUT_H * CONV2_OUT_W + 2*h * CONV2_OUT_W + 2*w];
                max_val = fmaxf(max_val, relu2_output[c * CONV2_OUT_H * CONV2_OUT_W + 2*h * CONV2_OUT_W + 2*w+1]);
                max_val = fmaxf(max_val, relu2_output[c * CONV2_OUT_H * CONV2_OUT_W + (2*h+1) * CONV2_OUT_W + 2*w]);
                max_val = fmaxf(max_val, relu2_output[c * CONV2_OUT_H * CONV2_OUT_W + (2*h+1) * CONV2_OUT_W + 2*w+1]);
                
                pool1_output[c * POOL1_OUT_H * POOL1_OUT_W + h * POOL1_OUT_W + w] = max_val;
            }
        }
    }
    
    // 4. 第二次池化: pool1_output -> pool2_output
    for (int c = 0; c < CONV2_OUT_CHANNELS; c++) {
        for (int h = 0; h < POOL2_OUT_H; h++) {
            for (int w = 0; w < POOL2_OUT_W; w++) {
                float max_val = pool1_output[c * POOL1_OUT_H * POOL1_OUT_W + 2*h * POOL1_OUT_W + 2*w];
                max_val = fmaxf(max_val, pool1_output[c * POOL1_OUT_H * POOL1_OUT_W + 2*h * POOL1_OUT_W + 2*w+1]);
                max_val = fmaxf(max_val, pool1_output[c * POOL1_OUT_H * POOL1_OUT_W + (2*h+1) * POOL1_OUT_W + 2*w]);
                max_val = fmaxf(max_val, pool1_output[c * POOL1_OUT_H * POOL1_OUT_W + (2*h+1) * POOL1_OUT_W + 2*w+1]);
                
                pool2_output[c * POOL2_OUT_H * POOL2_OUT_W + h * POOL2_OUT_W + w] = max_val;
            }
        }
    }
    
    // 5. 第一个全连接层: pool2_output (展平) -> fc1_output
    for (int i = 0; i < FC1_OUT; i++) {
        float sum = 0.0f;
        for (int j = 0; j < FC1_IN; j++) {
            sum += pool2_output[j] * fc1_weight[i * FC1_IN + j];
        }
        sum += fc1_bias[i];
        fc1_output[i] = sum > 0 ? sum : 0; // ReLU
    }
    
    // 6. 第二个全连接层: fc1_output -> output
    for (int i = 0; i < FC2_OUT; i++) {
        float sum = 0.0f;
        for (int j = 0; j < FC2_IN; j++) {
            sum += fc1_output[j] * fc2_weight[i * FC2_IN + j];
        }
        output[i] = sum + fc2_bias[i];
    }
}

void readbmp(const char *filename, float *img)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        printf("Can not open file %s\n", filename);
        return;
    }

    // 读取文件头（14字节）
    uint8_t file_header[14];
    fread(file_header, 1, 14, f);

    // 读取信息头（40字节）
    uint8_t info_header[40];
    fread(info_header, 1, 40, f);

    int width = *(int *)&info_header[4];
    int height = *(int *)&info_header[8];

    // 增加尺寸检查
    if (width != IMG_W || height != IMG_H)
    {
        printf("图像尺寸必须为 %dx%d, 当前图像为 %dx%d\n", IMG_W, IMG_H, width, height);
        fclose(f);
        return;
    }

    uint16_t bit_count = *(uint16_t *)&info_header[14];
    if (bit_count != 8)
    {
        printf("Only 8-bit grayscale BMP is supported.\n");
        fclose(f);
        return;
    }

    // 获取像素数据偏移
    uint32_t pixel_offset = *(uint32_t *)&file_header[10];
    fseek(f, pixel_offset, SEEK_SET);

    // 每行字节数4字节对齐
    int row_bytes = ((width + 3) / 4) * 4;
    uint8_t *row_buf = malloc(row_bytes);

    // BMP像素自下而上存储
    for (int y = height - 1; y >= 0; --y)
    {
        fread(row_buf, 1, row_bytes, f);
        for (int x = 0; x < width; ++x)
        {
            // 只做简单的归一化，后续在cnn_forward中会做标准化处理
            img[y * width + x] = row_buf[x] / 255.0f;
        }
    }

    free(row_buf);
    fclose(f);
}

// 读取大端整数（4字节）
uint32_t read_big_endian(FILE *fp)
{
    uint8_t bytes[4];
    fread(bytes, sizeof(uint8_t), 4, fp);
    return (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
}

// 读取图像数据
uint8_t **read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Failed to open image file");
        exit(1);
    }

    uint32_t magic = read_big_endian(fp);
    if (magic != 0x00000803)
    {
        fprintf(stderr, "Invalid image file magic number: %08X\n", magic);
        exit(1);
    }

    *num_images = read_big_endian(fp);
    *rows = read_big_endian(fp);
    *cols = read_big_endian(fp);

    uint8_t **images = malloc(*num_images * sizeof(uint8_t *));
    for (uint32_t i = 0; i < *num_images; ++i)
    {
        images[i] = malloc((*rows) * (*cols) * sizeof(uint8_t));
        fread(images[i], sizeof(uint8_t), (*rows) * (*cols), fp);
    }

    fclose(fp);
    return images;
}

// 读取标签数据
uint8_t *read_mnist_labels(const char *filename, uint32_t *num_labels)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        perror("Failed to open label file");
        exit(1);
    }

    uint32_t magic = read_big_endian(fp);
    if (magic != 0x00000801)
    {
        fprintf(stderr, "Invalid label file magic number: %08X\n", magic);
        exit(1);
    }

    *num_labels = read_big_endian(fp);
    uint8_t *labels = malloc(*num_labels * sizeof(uint8_t));
    fread(labels, sizeof(uint8_t), *num_labels, fp);

    fclose(fp);
    return labels;
}
