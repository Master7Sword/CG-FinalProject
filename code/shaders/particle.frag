#version 330 core
out vec4 FragColor;

in vec3 fragColor;
in float fragLuminance;

uniform vec3 lightPos;
uniform vec3 lightColor;

void main() {

    // 使用亮度调节粒子颜色
    vec3 result = fragColor * fragLuminance;
    FragColor = vec4(result, 1.0);
}
