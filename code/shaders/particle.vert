#version 330 core

layout(location = 0) in vec3 aPos;      // 粒子位置
layout(location = 1) in vec3 aColor;    // 粒子颜色
layout(location = 2) in float aLuminance; // 粒子亮度

out vec3 fragColor;
out float fragLuminance;

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(aPos, 1.0);
    fragColor = aColor;
    fragLuminance = aLuminance; // 将亮度传递到片段着色器
}
