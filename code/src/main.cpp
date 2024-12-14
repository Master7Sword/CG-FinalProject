#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <algorithm>
#include "Particle.h"
#include "ParticleRenderer.h"
#include "ObjLoader.h"
#include "Skybox.h"
#include "Camera.h"
#include "utils.h"

// 窗口大小调整回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}


// 处理按键输入
bool enterKeyPressed = false;
void processInput(GLFWwindow* window, float deltaTime, std::vector<Particle>& particles) {
    const float cameraSpeed = 1.0f * deltaTime; // 摄像机移动速度

    // 摄像机位置移动 (WASD)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Camera::move(Camera::front * cameraSpeed); // 向前
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Camera::move(-Camera::front * cameraSpeed); // 向后
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Camera::move(-glm::normalize(glm::cross(Camera::front, Camera::up)) * cameraSpeed); // 向左
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Camera::move(glm::normalize(glm::cross(Camera::front, Camera::up)) * cameraSpeed); // 向右
    }

    // 摄像机朝向调整（方向键）
    const float rotationSpeed = 50.0f * deltaTime; // 旋转速度
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        Camera::adjustPitch(rotationSpeed); // 向上看
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        Camera::adjustPitch(-rotationSpeed); // 向下看
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        Camera::adjustYaw(-rotationSpeed); // 向左看
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        Camera::adjustYaw(rotationSpeed); // 向右看
    }

    // 发射烟花
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!enterKeyPressed) {
            // 仅在按下时触发一次
            enterKeyPressed = true; // 标记为已按下

            Particle test;
            test.initialize(glm::vec3(0.0f, -15.0f, -30.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 12.0f, 0.0f), 
                            glm::vec3(1.0f, 0.0f, 0.0f), 1.0f, 100.0f, false, false, glm::vec3(0.0f,-0.981f, 0.0f));
            particles.push_back(test);
        }
    } else {
        // 当按键松开时重置标记
        enterKeyPressed = false;
    }
}


std::vector<Particle> particles;

void updateParticles(float deltaTime) {
    std::vector<Particle> newParticles;  // 用来存储新生成的粒子

    // 更新每个粒子的状态
    for (auto& particle : particles) {
        particle.update(deltaTime, newParticles);  // 将新生成的粒子存入 newParticles
    }

    // 将新生成的粒子添加到 particles
    particles.insert(particles.end(), newParticles.begin(), newParticles.end());

    // 删除TTL过期的粒子
    particles.erase(std::remove_if(particles.begin(), particles.end(), 
                                   [](const Particle& p) { return p.check_recycle(); }), 
                    particles.end());
}



int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    int window_width = 1200, window_height = 800;
    GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Skybox", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // 渲染对象
    Skybox skybox;
    skybox.initialize();

    ObjLoader objLoader;
    if (!objLoader.load("../../static/objects/Crate.obj", "../../static/objects")) {
        return -1;
    }
    glm::mat4 model = glm::mat4(1.0f); 

    ParticleRenderer particleRenderer;
    particleRenderer.initialize();

    float lastFrame = 0.0f; // 上一帧的时间
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime, particles);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = Camera::getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window_width) / window_height, 0.1f, 100.0f);
        
        // 先渲染固定的物体（天空盒、地面物体）
        skybox.render(view, projection);
        objLoader.render(view, projection, model);
        std::cout << "deltaTime: " << deltaTime << " FPS: " << 1.0 / deltaTime << " len(particles): " << particles.size() << std::endl;
        // 更新并渲染粒子
        updateParticles(deltaTime);
        particleRenderer.render(particles, view, projection); // 渲染粒子

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
