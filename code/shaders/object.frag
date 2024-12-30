#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    sampler2D diffuse;    // 漫反射贴图(同高光)
    sampler2D emission;   // 发射贴图
    sampler2D reflection; // 反射贴图
    sampler2D alpha;      // 透明度贴图
};

// 光源结构体
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

uniform Material material;
uniform vec3 viewPos;

// 光源数组
#define MAX_LIGHTS 100
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

#define AMBIENT 0.3
#define DIFFUSE 0.7
#define SPECULAR 0.5
#define SHININESS 32 // 高光强度

// 光线衰减函数
float calculateAttenuation(float distance) {
    // 衰减系数，可以根据需要调整
    float constant = 1.0;
    float linear = 100.0;
    float quadratic = 1.0;
    return 1.0 / (constant + linear * distance + quadratic * (distance * distance));
}

void main() {
    // 法线贴图处理
    vec3 norm = Normal;

    vec3 ambient = vec3(0.0); // 环境光
    vec3 diffuse = vec3(0.0); // 漫反射
    vec3 specular = vec3(0.0); // 镜面反射

    for (int i = 0; i < numLights; ++i) {
        // 计算光源到片段的距离
        float distance = length(lights[i].position - FragPos) + length(viewPos - FragPos) / 10;
        // 计算衰减系数
        float attenuation = calculateAttenuation(distance);

        // 环境光
        ambient += AMBIENT * vec3(texture(material.diffuse, TexCoord)) * lights[i].intensity * attenuation;

        // 漫反射光
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += DIFFUSE * diff * vec3(texture(material.diffuse, TexCoord)) * lights[i].intensity * attenuation;

        // 镜面高光
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), SHININESS);
        specular += SPECULAR * spec * vec3(texture(material.diffuse, TexCoord)) * lights[i].intensity * attenuation;
    }
    // 发射光
    vec3 emission = vec3(texture(material.emission, TexCoord));

    // 透明度
    float alpha = texture(material.alpha, TexCoord).r;
    if(alpha > 0.01) alpha = 1.0;

    // 最终颜色
    vec3 finalColor = ambient + diffuse + specular + emission;
    FragColor = vec4(finalColor, alpha);

}
