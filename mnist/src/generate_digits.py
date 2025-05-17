import numpy as np
from PIL import Image, ImageDraw, ImageFont
import os

# 手写数字图像生成工具

def create_digit_image(digit, filename, size=28):
    """创建一个手写数字的BMP图像
    
    参数:
        digit: 要生成的数字 (0-9)
        filename: 输出文件名
        size: 图像大小 (默认为28x28)
    """
    # 创建一个黑色背景图像
    img = Image.new('RGB', (size, size), color=(0, 0, 0))
    draw = ImageDraw.Draw(img)
    
    # 使用简单的文本方式绘制数字
    # 尝试找一个能表示手写风格的内置字体
    font_size = size - 6
    try:
        # 尝试使用一个通用字体
        font = ImageFont.truetype("Arial", font_size)
    except IOError:
        # 如果找不到字体，使用默认字体
        font = ImageFont.load_default()
    
    # 计算文本位置使其居中
    text = str(digit)
    text_width, text_height = draw.textbbox((0, 0), text, font=font)[2:4]
    position = ((size - text_width) // 2, (size - text_height) // 2)
    
    # 绘制数字
    draw.text(position, text, font=font, fill=(255, 255, 255))
    
    # 保存图像
    img.save(filename)
    print(f"已生成数字 {digit} 的图像: {filename}")

def main():
    # 确保images目录存在
    os.makedirs("images", exist_ok=True)
    
    # 为每个数字 (0-9) 生成一个BMP图像
    for digit in range(10):
        filename = f"images/test_{digit}.bmp"
        create_digit_image(digit, filename)

if __name__ == "__main__":
    main()
