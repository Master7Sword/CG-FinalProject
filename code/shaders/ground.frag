#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D groundTexture;
uniform vec3 viewPos;

// 光源结构体
struct Light {
    vec3 position;
    vec3 color;
    float intensity;
};

// 光源数组
#define MAX_LIGHTS 20
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

void main() {
    vec3 textureColor = texture(groundTexture, TexCoords).rgb;

    vec3 ambient = vec3(0.0); // 环境光
    vec3 diffuse = vec3(0.0); // 漫反射
    vec3 specular = vec3(0.0); // 镜面反射

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    for (int i = 0; i < numLights; ++i) {
        // 环境光
        float ambientStrength = 0.01;
        ambient += ambientStrength * lights[i].color * lights[i].intensity;

        // 漫反射
        float diffuseStrength = 0.75;
        vec3 lightDir = normalize(lights[i].position - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        diffuse += diffuseStrength * diff * lights[i].color * lights[i].intensity;
 
        // 镜面反射
        float specularStrength = 0.25;
        vec3 reflectDir = reflect(-lightDir, norm);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
        specular += specularStrength * spec * lights[i].color * lights[i].intensity;
    }

    vec3 result = (ambient + diffuse + specular) * textureColor;
    FragColor = vec4(result, 1.0);
}
