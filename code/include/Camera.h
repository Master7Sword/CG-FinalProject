#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

class Camera {
public:
    static glm::mat4 getViewMatrix();

    // 移动摄像机位置
    static void move(const glm::vec3& offset);

    // 调整摄像机角度
    static void adjustYaw(float offset);
    static void adjustPitch(float offset);

    static void updateCameraVectors(); // 更新摄像机方向向量

    static glm::vec3 position;
    static glm::vec3 front;
    static glm::vec3 up;

    static float yaw;   // 偏航角
    static float pitch; // 俯仰角
};

#endif
