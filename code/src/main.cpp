#include <iostream>
#include <SFML/Audio.hpp>
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

            launchSound[launch_index].play(); // 发射音效
            launch_index = (launch_index + 1) % MAX_SOUNDS;

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

std::vector<Particle> particles; // 存储所有粒子

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

    // 初始化音效
    for (int i = 0; i < MAX_SOUNDS; ++i){
        if (!launchBuffer[i].loadFromFile("../../static/audio/launch.wav")) {
            std::cerr << "Failed to load launch.wav" << std::endl;
            return -1;
        }
        launchSound[i].setBuffer(launchBuffer[i]);
        if (!explosionBuffer[i].loadFromFile("../../static/audio/explosion.wav")) {
            std::cerr << "Failed to load explosion sound!" << std::endl;
            return -1;
        }
        explosionSound[i].setBuffer(explosionBuffer[i]);
    }
    

    // 渲染对象
    Skybox skybox;
    skybox.initialize();

    ObjLoader objLoader;
    if (!objLoader.load("../../static/objects/Crate.obj", "../../static/objects")) {
        return -1;
    }

    ParticleRenderer particleRenderer;
    particleRenderer.initialize();

    float lastFrame = 0.0f; // 上一帧的时间
    while (!glfwWindowShouldClose(window)) {
        auto frameStart = Clock::now();

        // 1. 计算帧时间
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        std::cout << "deltaTime: " << deltaTime << " FPS: " << 1.0 / deltaTime << std::endl;

        auto inputStart = Clock::now();
        // 2. 处理输入
        processInput(window, deltaTime, particles);
        measureTime("输入处理", inputStart);

        auto clearStart = Clock::now();
        // 3. 清屏
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        measureTime("清屏", clearStart);

        auto renderStart = Clock::now();
        // 4. 渲染固定物体
        glm::mat4 view = Camera::getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window_width) / window_height, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f); 
        skybox.render(view, projection);
        objLoader.render(view, projection, model);
        measureTime("固定物体渲染", renderStart);

        auto particleUpdateStart = Clock::now();
        // 5. 更新粒子
        updateParticles(deltaTime, particles);
        measureTime("粒子更新", particleUpdateStart);

        auto particleRenderStart = Clock::now();
        // 6. 渲染粒子
        particleRenderer.render(particles, view, projection);
        measureTime("粒子渲染", particleRenderStart);

        auto bufferSwapStart = Clock::now();
        // 7. 交换缓冲区和轮询事件
        glfwSwapBuffers(window);
        glfwPollEvents();
        measureTime("缓冲交换和事件处理", bufferSwapStart);

        measureTime("完整帧", frameStart);
    }


    glfwTerminate();
    return 0;
}
