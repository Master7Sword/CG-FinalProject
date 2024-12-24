import cv2
import os
import sys
import numpy as np

if len(sys.argv) < 2:
    print("Usage: python gen.py <image_path>\nFor example: python gen.py MSC.png")
    exit()

image_path = sys.argv[1]  # 获取文件名参数
image = cv2.imread(image_path)

if image is None:
    print(f"Can not read file: {image_path}")
    exit()

height, width, _ = image.shape
basePos = "basePos"
size = 0.1
positions = []

for y in range(0, height, 10):
    for x in range(0, width, 10):
        pixel = image[y, x]
        if not np.array_equal(pixel, [0, 0, 0]):
            positions.append(f"{basePos} + size*glm::vec3({x * 0.1}, {(height - 1 - y) * 0.1}, 0)")  # Y轴翻转

output_filename = os.path.splitext(image_path)[0] + '.txt'

with open(output_filename, 'w') as f:
    f.write("std::vector<glm::vec3> pattern = {\n")
    for pos in positions:
        f.write(f"\t{pos},\n")
    f.write("};\n")

print(f"{image_path} vertex info has been saved to: {output_filename}")