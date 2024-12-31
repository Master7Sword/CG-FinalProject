import cv2
import os
import sys
import numpy as np

step = 200
size = 0.1

if len(sys.argv) < 2:
    print("Usage: python gen.py <image_path>\nFor example: python gen.py msc.png")
    exit()

image_path = sys.argv[1]  # 获取文件名参数
image = cv2.imread(image_path, cv2.IMREAD_UNCHANGED)

if image is None:
    print(f"Can not read file: {image_path}")
    exit()

height, width, _ = image.shape
positions = []
rgbs = []

for y in range(0, height, step):
    for x in range(0, width, step):
        pixel = image[y, x]
        # if not np.array_equal(pixel, [0, 0, 0]):  # 黑色背景
        if len(pixel) == 4 and pixel[3] != 0:  # 透明背景
            r, g, b = pixel[0], pixel[1], pixel[2]  # 获取 RGB 值
            positions.append(f"base + size * glm::vec3({(x - width / 2) * size}, {(height / 2 - y) * size}, 0)")  # 以图案中心为坐标原点, y 轴镜像旋转
            rgbs.append(f"glm::vec3({r / 255.0}, {g / 255.0}, {b / 255.0})")

output_filename = os.path.splitext(image_path)[0] + '.h'

with open(output_filename, 'w') as f:
    f.write(f"#include<glm/glm.hpp>\n#include<vector>\n\n")
    f.write(f"inline std::vector<glm::vec3> {os.path.splitext(image_path)[0]}_positions(glm::vec3 base, float size) {{\n  return {{\n")
    for pos in positions:
        f.write(f"    {pos},\n")
    f.write("  };\n}\n\n")
    f.write(f"inline std::vector<glm::vec3> {os.path.splitext(image_path)[0]}_rgbs() {{\n  return {{\n")
    for rgb in rgbs:
        f.write(f"    {rgb},\n")
    f.write("  };\n}")

print(f"{image_path} vertex info has been saved to: {output_filename}")