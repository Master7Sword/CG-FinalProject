#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <SFML/Audio.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <chrono>
#include <glm/glm.hpp>
#include "stb_image.h"

#define MAX_SOUNDS 10

// 发射音效
extern sf::SoundBuffer launchBuffer[MAX_SOUNDS];
extern sf::SoundBuffer explosionBuffer[MAX_SOUNDS];
extern sf::Sound launchSound[MAX_SOUNDS];
extern sf::Sound explosionSound[MAX_SOUNDS];
extern int launch_index;
extern int explosion_index;

GLuint loadShader(const char* vertexPath, const char* fragmentPath);
GLuint loadTexture(const std::string& path);
std::string readFile(const std::string& filePath);
glm::vec3 getRandomColor();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

using Clock = std::chrono::high_resolution_clock;
void measureTime(const std::string &stageName, const Clock::time_point &start);

#endif
