#include "Camera.h"

// 构造函数：使用向量
Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM)
{
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

// 构造函数：使用标量
Camera::Camera(float posX, float posY, float posZ,
               float upX, float upY, float upZ,
               float yaw, float pitch)
    : Front(glm::vec3(0.0f, 0.0f, -1.0f)),
      MovementSpeed(SPEED),
      MouseSensitivity(SENSITIVITY),
      Zoom(ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = yaw;
    Pitch = pitch;
    UpdateCameraVectors();
}

// 获取视图矩阵
glm::mat4 Camera::GetViewMatrix() const
{
    // lookAt函数：相机位置，目标位置（位置+前方向），上方向
    return glm::lookAt(Position, Position + Front, Up);
}

// 处理键盘输入
void Camera::ProcessKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;

    // 根据方向移动相机
    if (direction == FORWARD)
        Position += Front * velocity;
    if (direction == BACKWARD)
        Position -= Front * velocity;
    if (direction == LEFT)
        Position -= Right * velocity;
    if (direction == RIGHT)
        Position += Right * velocity;
    if (direction == UP)
        Position += Up * velocity;
    if (direction == DOWN)
        Position -= Up * velocity;
}

// 处理鼠标移动
void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch)
{
    // 应用鼠标灵敏度
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    // 更新欧拉角
    Yaw += xoffset;
    Pitch += yoffset;

    // 限制俯仰角，防止屏幕翻转
    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    // 更新相机向量
    UpdateCameraVectors();
}

// 处理鼠标滚轮（用于缩放）
void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= yoffset;

    // 限制缩放范围
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

// 更新相机向量
void Camera::UpdateCameraVectors()
{
    // 从欧拉角计算新的前向量
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);

    // 重新计算右向量和上向量
    // 右向量 = 前向量 × 世界上向量（叉积）
    Right = glm::normalize(glm::cross(Front, WorldUp));

    // 上向量 = 右向量 × 前向量
    Up = glm::normalize(glm::cross(Right, Front));
}
