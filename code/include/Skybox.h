#ifndef SKYBOX_H
#define SKYBOX_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "stb_image.h"

class Skybox {
public:
    void initialize();
    void render(const glm::mat4& view, const glm::mat4& projection);

private:
    GLuint skyboxVAO, skyboxVBO, cubemapTexture;
    GLuint shaderProgram;

    std::vector<std::string> faces = {
        "assets/textures/right.jpg",
        "assets/textures/left.jpg",
        "assets/textures/top.jpg",
        "assets/textures/bottom.jpg",
        "assets/textures/front.jpg",
        "assets/textures/back.jpg"
    };

    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif
