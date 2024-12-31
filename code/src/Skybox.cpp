#include "Skybox.h"
#include "stb_image.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



void Skybox::initialize() {
    skybox_size = 60.0;
    float skyboxVertices[] = {      
    -skybox_size,  skybox_size, -skybox_size,
    -skybox_size, -skybox_size, -skybox_size,
    skybox_size, -skybox_size, -skybox_size,
    skybox_size, -skybox_size, -skybox_size,
    skybox_size,  skybox_size, -skybox_size,
    -skybox_size,  skybox_size, -skybox_size,

    -skybox_size, -skybox_size,  skybox_size,
    -skybox_size, -skybox_size, -skybox_size,
    -skybox_size,  skybox_size, -skybox_size,
    -skybox_size,  skybox_size, -skybox_size,
    -skybox_size,  skybox_size,  skybox_size,
    -skybox_size, -skybox_size,  skybox_size,

     skybox_size, -skybox_size, -skybox_size,
     skybox_size, -skybox_size,  skybox_size,
     skybox_size,  skybox_size,  skybox_size,
     skybox_size,  skybox_size,  skybox_size,
     skybox_size,  skybox_size, -skybox_size,
     skybox_size, -skybox_size, -skybox_size,

    -skybox_size, -skybox_size,  skybox_size,
    -skybox_size,  skybox_size,  skybox_size,
     skybox_size,  skybox_size,  skybox_size,
     skybox_size,  skybox_size,  skybox_size,
     skybox_size, -skybox_size,  skybox_size,
    -skybox_size, -skybox_size,  skybox_size,

    -skybox_size,  skybox_size, -skybox_size,
     skybox_size,  skybox_size, -skybox_size,
     skybox_size,  skybox_size,  skybox_size,
     skybox_size,  skybox_size,  skybox_size,
    -skybox_size,  skybox_size,  skybox_size,
    -skybox_size,  skybox_size, -skybox_size,

    -skybox_size, -skybox_size, -skybox_size,
    -skybox_size, -skybox_size,  skybox_size,
     skybox_size, -skybox_size, -skybox_size,
     skybox_size, -skybox_size, -skybox_size,
    -skybox_size, -skybox_size,  skybox_size,
     skybox_size, -skybox_size,  skybox_size
};

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    cubemapTexture = loadCubemap(faces);
    shaderProgram = loadShader("../shaders/skybox.vert", "../shaders/skybox.frag");
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection) {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(shaderProgram);
    glm::mat4 skyboxView = glm::mat4(glm::mat3(view)); // remove translation
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(skyboxView));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

GLuint Skybox::loadCubemap(const std::vector<std::string>& faces) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (GLuint i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            return 0;
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


GLuint Skybox::loadShader(const char* vertexPath, const char* fragmentPath) {
    // 1. 从文件中读取源代码
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    
    try {
        // 打开文件
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲区内容到流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // 关闭文件
        vShaderFile.close();
        fShaderFile.close();
        // 将流转换为字符串
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure& e) {
        std::cerr << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        return 0;
    }
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    // 2. 编译顶点着色器
    GLuint vertex, fragment;
    GLint success;
    GLchar infoLog[512];

    // 顶点着色器
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // 检查编译是否成功
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 片段着色器
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // 检查编译是否成功
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 3. 着色器程序
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertex);
    glAttachShader(shaderProgram, fragment);
    glLinkProgram(shaderProgram);
    // 检查链接是否成功
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        return 0;
    }

    // 删除着色器，因为它们已经链接到程序中了
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderProgram;
}
