#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 overrideColor; // 颜色变量
uniform bool useOverrideColor; // 是否使用覆盖颜色
uniform float transparency;

void main() {
    if (useOverrideColor) {
        FragColor = vec4(overrideColor, transparency);
    } else {
        FragColor = texture(texture1, TexCoord);
    }
}
