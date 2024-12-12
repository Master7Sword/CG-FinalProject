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
        "../../static/textures/right.jpg",
        "../../static/textures/left.jpg",
        "../../static/textures/top.jpg",
        "../../static/textures/bottom.jpg",
        "../../static/textures/front.jpg",
        "../../static/textures/back.jpg"
    };

    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif
