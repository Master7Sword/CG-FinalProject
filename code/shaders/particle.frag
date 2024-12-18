#version 330 core
out vec4 FragColor;

uniform vec3 overrideColor; // 颜色变量
uniform float transparency;

void main() {
    FragColor = vec4(overrideColor, transparency);
}
