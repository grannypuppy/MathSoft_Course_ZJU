#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>

// 参数维度常量
#define W1_ROWS 128
#define W1_COLS 784
#define B1_LEN 128
#define W2_ROWS 10
#define W2_COLS 128
#define B2_LEN 10
#define IMG_W 28

uint32_t read_big_endian(FILE *fp);

uint8_t** read_mnist_images(const char *filename, uint32_t *num_images, uint32_t *rows, uint32_t *cols);

uint8_t* read_mnist_labels(const char *filename, uint32_t *num_labels);

void mnist_forward(float *x,
                   float *W1, float *b1,
                   float *W2, float *b2,
                   float *y2);

void readbmp(const char* filename, float* img);                   

int main(int argc, char* argv[])
{
    assert(sizeof(float) == 4); // 通不过则报错

    FILE *fp = fopen("parameters_mlp.bin", "rb");
    if (!fp)
    {
        printf("无法打开参数文件！\n");
        return 1;
    }

    // 分配内存
    float *W1 = (float *)malloc(sizeof(float) * W1_ROWS * W1_COLS);
    float *b1 = (float *)malloc(sizeof(float) * B1_LEN);
    float *W2 = (float *)malloc(sizeof(float) * W2_ROWS * W2_COLS);
    float *b2 = (float *)malloc(sizeof(float) * B2_LEN);

    if (!W1 || !b1 || !W2 || !b2)
    {
        printf("内存分配失败！\n");
        fclose(fp);
        return 2;
    }

    // 依次读取参数
    size_t nread = fread(W1, sizeof(float), W1_ROWS * W1_COLS, fp);
    if (nread != W1_ROWS * W1_COLS)
    {
        printf("W1读取失败\n");
        return 3;
    }
    nread = fread(b1, sizeof(float), B1_LEN, fp);
    if (nread != B1_LEN)
    {
        printf("b1读取失败\n");
        return 4;
    }
    nread = fread(W2, sizeof(float), W2_ROWS * W2_COLS, fp);
    if (nread != W2_ROWS * W2_COLS)
    {
        printf("W2读取失败\n");
        return 5;
    }
    nread = fread(b2, sizeof(float), B2_LEN, fp);
    if (nread != B2_LEN)
    {
        printf("b2读取失败\n");
        return 6;
    }

    fclose(fp);

    if (argc == 2)
    {
        // 将图像数据转换为浮点数
        float img[IMG_W * IMG_W];
        float y2[10]; // 输出数据

        readbmp(argv[1], img);

        mnist_forward(img, W1, b1, W2, b2, y2);
        int number = 0;
        for (int i = 0; i < 10; ++i)
        {
           if (y2[number] < y2[i])
               number = i;
        }

        printf("The number is %d\n", number);
    }
    else
    {
        const char *image_file = "./data/t10k-images-idx3-ubyte";
        const char *label_file = "./data/t10k-labels-idx1-ubyte";

        uint32_t num_images, rows, cols, num_labels;
        uint8_t **images = read_mnist_images(image_file, &num_images, &rows, &cols);
        uint8_t *labels = read_mnist_labels(label_file, &num_labels);

        int correct = 0;
        for (int i = 0; i < num_images; ++i) {
            float y2[10]; // 输出数据
            // 将图像数据转换为浮点数
            float img[IMG_W * IMG_W];
            for (int j = 0; j < IMG_W * IMG_W; ++j)
            {
                img[j] = images[i][j] / 255.0f;
            }

            mnist_forward(img, W1, b1, W2, b2, y2);
            int number = 0;
            for (int i = 0; i < 10; ++i)
            {
                if (y2[number] < y2[i])
                    number = i;
            }
            if (number == labels[i])
                correct++;
        }
        
        printf("The correction is: %f\n", correct * 1.0 / num_images);
    }
    // 释放内存
    free(W1);
    free(b1);
    free(W2);
    free(b2);

    return 0;
}

// 输入：x[28][28]，网络参数W1[128][784], b1[128], W2[10][128], b2[10]
// 输出：y2[10]
void mnist_forward(float *x,
                   float *W1, float *b1,
                   float *W2, float *b2,
                   float *y2)
{

    // 2. fc1
    float y1[128];
    for (int i = 0; i < 128; ++i)
    {
        float acc = 0.0f;
        for (int j = 0; j < 784; ++j)
        {
            acc += W1[i * 784 + j] * x[j];
        }
        y1[i] = acc + b1[i];
    }

    // 3. relu
    for (int i = 0; i < 128; ++i)
        if (y1[i] < 0)
            y1[i] = 0;

    // 4. fc2
    for (int i = 0; i < 10; ++i)
    {
        float acc = 0.0f;
        for (int j = 0; j < 128; ++j)
        {
            acc += W2[i * 128 + j] * y1[j];
        }
        y2[i] = acc + b2[i];
    }
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

