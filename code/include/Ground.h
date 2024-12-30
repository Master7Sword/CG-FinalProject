#ifndef GROUND_H
#define GROUND_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <vector>
#include "Camera.h"
#include "Light.h"
#include "stb_image.h"


class Ground {
public:
    void initialize(const std::string& texturePath);
    void render(const glm::mat4& view, const glm::mat4& projection, std::vector<Light> lights, Light env_light);

private:
    GLuint groundVAO, groundVBO, groundTexture;
    GLuint shaderProgram;

    GLuint loadTexture(const std::string& path);
    GLuint loadShader(const char* vertexPath, const char* fragmentPath);
};

#endif
