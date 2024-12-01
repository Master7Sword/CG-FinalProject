#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "ObjLoader.h"
#include "Skybox.h"
#include "Camera.h"
#include "utils.h"

// 窗口大小调整回调
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// 处理按键输入
void processInput(GLFWwindow* window, float deltaTime) {
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

    Skybox skybox;
    skybox.initialize();

    ObjLoader objLoader;
    if (!objLoader.load("../static/objects/Crate.obj", "../static/objects")) {
        return -1;
    }

    glm::mat4 model = glm::mat4(1.0f); // 设置物体的模型矩阵

    float lastFrame = 0.0f; // 上一帧的时间

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window, deltaTime);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = Camera::getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window_width) / window_height, 0.1f, 100.0f);
        
        skybox.render(view, projection);
        objLoader.render(view, projection, model);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
