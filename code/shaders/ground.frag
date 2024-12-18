#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // 从顶点着色器传来的纹理坐标

uniform sampler2D groundTexture; // 地面的二维纹理

void main() {
    FragColor = texture(groundTexture, TexCoords); // 根据纹理坐标采样纹理
}
