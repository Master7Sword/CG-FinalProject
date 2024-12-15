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
        "../../static/skybox_textures/right.png",
        "../../static/skybox_textures/left.png",
        "../../static/skybox_textures/top.png",
        "../../static/skybox_textures/bottom.png",
        "../../static/skybox_textures/front.png",
        "../../static/skybox_textures/back.png"
    };

    GLuint loadCubemap(const std::vector<std::string>& faces);
    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif
