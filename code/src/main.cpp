#include <iostream>
#include <vector>
#include <algorithm>
#include <SFML/Audio.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "utils.h"
#include "Light.h"
#include "Camera.h"
#include "Skybox.h"
#include "Ground.h"
#include "ObjLoader.h"
#include "Particle.h"
#include "Fireworks.h"
#include "ParticleRenderer.h"


// 处理按键输入
bool enterKeyPressed = false;
void processInput(GLFWwindow *window, float deltaTime, Fireworks & firework)
{
    const float cameraSpeed = 1.0f * deltaTime; // 摄像机移动速度

    // 摄像机位置移动 (WASD)
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Camera::move(Camera::front * cameraSpeed); // 向前
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Camera::move(-Camera::front * cameraSpeed); // 向后
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Camera::move(-glm::normalize(glm::cross(Camera::front, Camera::up)) * cameraSpeed); // 向左
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Camera::move(glm::normalize(glm::cross(Camera::front, Camera::up)) * cameraSpeed); // 向右
    }

    // 摄像机朝向调整（方向键）
    const float rotationSpeed = 50.0f * deltaTime; // 旋转速度
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        Camera::adjustPitch(rotationSpeed); // 向上看
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        Camera::adjustPitch(-rotationSpeed); // 向下看
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        Camera::adjustYaw(-rotationSpeed); // 向左看
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        Camera::adjustYaw(rotationSpeed); // 向右看
    }

    // 切换烟花
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
    {
        firework.switchFirework(GLFW_KEY_1);
    }

    // 发射烟花
    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
    {
        if (!enterKeyPressed)
        {
            // 仅在按下时触发一次
            enterKeyPressed = true; // 标记为已按下
            launchSound[launch_index].play(); // 发射音效
            launch_index = (launch_index + 1) % MAX_SOUNDS;
            firework.launch();
        }
    }
    else
    {
        // 当按键松开时重置标记
        enterKeyPressed = false;
    }
}

std::vector<Light> lights = {};                                                        // 烟花产生的光源，动态调整
Light env_light = {{0.0f, 1000.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, 0.5f, 114514.1919810f}; // 预设月光，每次渲染时加入lights中

int main()
{
    // 初始化 GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // 指定OpenGL版本为3.3，核心模式
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口
    int window_width = 1200, window_height = 800;
    GLFWwindow *window = glfwCreateWindow(window_width, window_height, "Firework", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    // 设置当前窗口为当前上下文，指定窗口大小变化时的回调函数，设置鼠标回调
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 初始化 GLEW
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    // 初始化音效
    for (int i = 0; i < MAX_SOUNDS; ++i)
    {
        if (!launchBuffer[i].loadFromFile("../../static/audio/launch.wav"))
        {
            std::cerr << "Failed to load launch.wav" << std::endl;
            return -1;
        }
        launchSound[i].setBuffer(launchBuffer[i]);
        if (!explosionBuffer[i].loadFromFile("../../static/audio/explosion.wav"))
        {
            std::cerr << "Failed to load explosion sound!" << std::endl;
            return -1;
        }
        explosionSound[i].setBuffer(explosionBuffer[i]);
    }

    // 加载并初始化静态对象
    Skybox skybox;
    skybox.initialize();

    Ground ground;
    ground.initialize("../../static/ground_textures/ground_stone.png");

    ObjLoader shrine;
    if (!shrine.load("../../static/objects/shrine.obj", "../../static/objects", "../shaders/object.vert", "../shaders/object.frag"))
    {
        return -1;
    }
    ObjLoader sky;
    if (!sky.load("../../static/objects/sky.obj", "../../static/objects", "../shaders/object.vert", "../shaders/object.frag")) {
        return -1;
    }
    // ObjLoader test;
    // if (!test.load("../../static/objects/test.obj", "../../static/objects", "../shaders/object.vert", "../shaders/object.frag")) {
    //     return -1;
    // }


    Fireworks firework;
    firework.initialize();
    float lastFrame = 0.0f; // 上一帧的时间

    // 主循环
    while (!glfwWindowShouldClose(window))
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        auto frameStart = Clock::now();

        // 1. 计算帧时间
        float currentFrame = glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        char buffer[256];
        snprintf(
            buffer,
            sizeof(buffer),
            "       DeltaTime : %8.2f\n             FPS : %8d\n       Particles : %8ld\n          Lights : %8ld\n------------------------------\n",
            deltaTime,
            int(1.0 / deltaTime),
            firework.numParticles(),
            lights.size()+1);
        std::string frame_log(buffer);

        // 2. 处理输入
        auto inputStart = Clock::now();
        processInput(window, deltaTime, firework);
        frame_log += measureTime("Process Input", inputStart);

        // 3. 清屏
        auto clearStart = Clock::now();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        frame_log += measureTime("GL Clear", clearStart);

        // 4. 渲染固定背景
        auto renderStart = Clock::now();
        glm::mat4 view = Camera::getViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window_width) / window_height, 0.1f, 100.0f);      
        skybox.render(view, projection);
        ground.render(view, projection, lights, env_light);
        frame_log += measureTime("Object Render", renderStart);

        // 5. 更新粒子
        auto particleUpdateStart = Clock::now();
        firework.update(deltaTime, lights);
        frame_log += measureTime("Particle Update", particleUpdateStart);

        // 6. 渲染粒子
        auto particleRenderStart = Clock::now();
        firework.render(view, projection);
        frame_log += measureTime("Particle Render", particleRenderStart);

        glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
        sky.renderWithTexture(view, projection, model, lights, env_light);
        shrine.renderWithTexture(view, projection, model, lights, env_light);
        // test.renderWithTexture(view, projection, model, lights, env_light);

        // 8. 交换缓冲区和轮询事件
        auto bufferSwapStart = Clock::now();
        glfwSwapBuffers(window);
        glfwPollEvents();


        frame_log += measureTime("Buffer & Event", bufferSwapStart);
        frame_log += measureTime("Total Frame", frameStart);
        std::cout << "\033[2J\033[1;1H"; // ANSI 清屏序列
        std::cout << frame_log;
    }

    glfwTerminate();
    return 0;
}
