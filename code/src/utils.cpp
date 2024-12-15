#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


sf::SoundBuffer launchBuffer[10];
sf::SoundBuffer explosionBuffer[10];
sf::Sound launchSound[10];
sf::Sound explosionSound[10];
int launch_index = 0;
int explosion_index = 0;


// 窗口大小调整回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << filePath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint loadShader(const char* vertexPath, const char* fragmentPath) {
    // Shader loading and compilation logic
    GLuint vertex, fragment;
    std::ifstream vShaderFile, fShaderFile;
    vShaderFile.open(vertexPath);
    fShaderFile.open(fragmentPath);

    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();

    vShaderFile.close();
    fShaderFile.close();

    const std::string& vertexCode = vShaderStream.str();
    const std::string& fragmentCode = fShaderStream.str();

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return program;
}

GLuint loadTexture(const std::string& path) {
    // Texture loading logic
    GLuint textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
    stbi_image_free(data);

    return textureID;
}

glm::vec3 getRandomColor() {
    float r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);  // 随机生成 [0, 1]
    float g = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);  // 随机生成 [0, 1]
    float b = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);  // 随机生成 [0, 1]
    return glm::vec3(r, g, b);  // 返回生成的随机颜色
}


void measureTime(const std::string &stageName, const Clock::time_point &start) {
    auto end = Clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << stageName << "耗时: " << duration << " 微秒" << std::endl;
}