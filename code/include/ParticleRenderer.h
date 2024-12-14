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
    GLuint shaderProgram;
    GLuint framebuffer;
    GLuint textureColorBuffer;
    GLuint rbo;
    ObjLoader sphereModel; // 小球模型

    struct ParticleVertex {
        glm::vec3 position;
        glm::vec3 color;
        float luminance;
    };

    GLuint compileShader(const char* source, GLenum shaderType);
    GLuint loadShaders(const char* vertexPath, const char* fragmentPath);
    
};

#endif
