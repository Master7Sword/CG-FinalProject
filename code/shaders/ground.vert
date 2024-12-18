#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal; // 添加法向量

out vec2 TexCoords;
out vec3 FragPos;  // 片段的位置
out vec3 Normal;   // 法向量

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0)); // 世界坐标系中的片段位置
    Normal = mat3(transpose(inverse(model))) * aNormal; // 转换法向量到世界坐标系
    TexCoords = aTexCoords;
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
