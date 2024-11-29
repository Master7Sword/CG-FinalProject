#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>

glm::vec3 Camera::position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 Camera::front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 Camera::up = glm::vec3(0.0f, 1.0f, 0.0f);

float Camera::yaw = -90.0f; // 初始偏航角
float Camera::pitch = 0.0f; // 初始俯仰角

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

void Camera::move(const glm::vec3& offset) {
    position += offset;
}

void Camera::adjustYaw(float offset) {
    yaw += offset;
    updateCameraVectors();
}

void Camera::adjustPitch(float offset) {
    pitch += offset;

    // 限制俯仰角范围
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    // 更新摄像机方向向量
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
}
