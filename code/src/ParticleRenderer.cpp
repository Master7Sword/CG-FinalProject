#include "ParticleRenderer.h"
#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "utils.h"

ParticleRenderer::ParticleRenderer() : VAO(0), VBO(0), shaderProgram(0) {}

ParticleRenderer::~ParticleRenderer() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (shaderProgram) glDeleteProgram(shaderProgram);
}

GLuint ParticleRenderer::compileShader(const char* source, GLenum shaderType) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Failed: " << infoLog << std::endl;
    }
    return shader;
}

GLuint ParticleRenderer::loadShaders(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader Linking Failed: " << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void ParticleRenderer::initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float) * 7, (void*)(sizeof(float) * 6));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    sphereModel.load("../../static/objects/sphere(r=0.5).obj", "../../static/objects");

    // 启用混合模式
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}



void ParticleRenderer::render(const std::vector<Particle>& particles, const glm::mat4& view, const glm::mat4& projection) {
    glUseProgram(shaderProgram);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    for (const auto& particle : particles) {
        glm::mat4 model = glm::translate(glm::mat4(1.0f), particle.getPosition());
        model = glm::scale(model, glm::vec3(0.1f));  // 控制球体的大小

        // 使用带颜色的渲染
        sphereModel.renderWithColor(view, projection, model, particle.getColor(),  particle.getTransparency());
    }
}
