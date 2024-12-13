#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 overrideColor; // 新增的颜色变量
uniform bool useOverrideColor; // 是否使用覆盖颜色

void main() {
    if (useOverrideColor) {
        FragColor = vec4(overrideColor, 1.0);
    } else {
        FragColor = texture(texture1, TexCoord);
    }
}
