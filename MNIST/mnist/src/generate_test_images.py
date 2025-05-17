#!/usr/bin/env python3
# 生成手写数字BMP图片样例

import numpy as np
from PIL import Image, ImageDraw
import os

# 创建目录（如果不存在）
os.makedirs("../images", exist_ok=True)

# 图片大小
width, height = 28, 28

# 每个数字的简单像素模板 (0-9)
# 这些是数字的简化表示，1代表绘制的像素
templates = {
    0: [
        "  ##### ",
        " #     #",
        "#      #",
        "#      #",
        "#      #",
        "#      #",
        "#      #",
        " #     #",
        "  ##### "
    ],
    1: [
        "    #   ",
        "   ##   ",
        "  # #   ",
        "    #   ",
        "    #   ",
        "    #   ",
        "    #   ",
        "    #   ",
        "  ##### "
    ],
    2: [
        " ###### ",
        "#      #",
        "       #",
        "      # ",
        "    ##  ",
        "  ##    ",
        " #      ",
        "#       ",
        "########"
    ],
    3: [
        " ###### ",
        "#      #",
        "       #",
        "     ## ",
        "       #",
        "       #",
        "       #",
        "#      #",
        " ###### "
    ],
    4: [
        "     #  ",
        "    ##  ",
        "   # #  ",
        "  #  #  ",
        " #   #  ",
        "########",
        "     #  ",
        "     #  ",
        "     #  "
    ],
    5: [
        "########",
        "#       ",
        "#       ",
        "###### ",
        "       #",
        "       #",
        "       #",
        "#      #",
        " ###### "
    ],
    6: [
        "  ##### ",
        " #      ",
        "#       ",
        "###### ",
        "#      #",
        "#      #",
        "#      #",
        " #     #",
        "  ##### "
    ],
    7: [
        "########",
        "       #",
        "      # ",
        "     #  ",
        "    #   ",
        "   #    ",
        "  #     ",
        " #      ",
        "#       "
    ],
    8: [
        " ###### ",
        "#      #",
        "#      #",
        " ###### ",
        "#      #",
        "#      #",
        "#      #",
        "#      #",
        " ###### "
    ],
    9: [
        " ###### ",
        "#      #",
        "#      #",
        "#      #",
        " #######",
        "       #",
        "       #",
        "#      #",
        " ###### "
    ]
}

# 为每个数字(0-9)创建一个BMP图片
for digit in range(10):
    # 创建一个白色背景图片 (255 = 白色)
    image = Image.new('L', (width, height), 255)
    pixels = image.load()
    
    # 获取当前数字的模板
    template = templates[digit]
    
    # 计算居中的偏移量
    template_height = len(template)
    template_width = len(template[0])
    offset_x = (width - template_width) // 2
    offset_y = (height - template_height) // 2
    
    # 根据模板绘制数字
    for y in range(template_height):
        for x in range(template_width):
            try:
                if y < len(template) and x < len(template[y]) and template[y][x] != ' ':
                    pixels[x + offset_x, y + offset_y] = 0  # 黑色像素
            except IndexError:
                pass  # 忽略越界错误
    
    # 保存为BMP格式
    filename = f"../images/test_{digit}.bmp"
    image.save(filename)
    print(f"已创建: {filename}")

print("所有测试图片已创建完成")
