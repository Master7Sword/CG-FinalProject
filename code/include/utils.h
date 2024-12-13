#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <GL/glew.h>
#include "stb_image.h"
#include <glm/glm.hpp>

GLuint loadShader(const char* vertexPath, const char* fragmentPath);
GLuint loadTexture(const std::string& path);
std::string readFile(const std::string& filePath);
glm::vec3 getRandomColor();

#endif
