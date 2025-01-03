#ifndef PARTICLE_RENDERER_H
#define PARTICLE_RENDERER_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "Particle.h"
#include "ObjLoader.h"

class ParticleRenderer {
public:
    ParticleRenderer();
    ~ParticleRenderer();

    void initialize();
    void render(const std::vector<Particle>& particles, const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint VAO, VBO;
    GLuint framebuffer;
    GLuint textureColorBuffer;
    ObjLoader sphereModel; // 小球模型

    GLuint compileShader(const char* source, GLenum shaderType);
    GLuint loadShaders(const char* vertexPath, const char* fragmentPath);
};

#endif
