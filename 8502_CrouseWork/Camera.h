#ifndef CAMERA_H
#define CAMERA_H

#include "nclgl/Matrix4.h"
#include "nclgl/Vector3.h"

// 相机移动方向枚举
enum CameraMovement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// 默认相机参数
const float YAW = -90.0f;           // 偏航角（左右看）
const float PITCH = 0.0f;           // 俯仰角（上下看）
const float SPEED = 20.0f;          // 移动速度
const float SENSITIVITY = 0.1f;     // 鼠标灵敏度
const float ZOOM = 45.0f;           // 视野角度

// Camera类：实现第一人称相机
class Camera
{
public:
    // 相机属性
    Vector3 Position;      // 相机位置
    Vector3 Front;         // 相机朝向（前方向量）
    Vector3 Up;            // 相机上方向
    Vector3 Right;         // 相机右方向
    Vector3 WorldUp;       // 世界上方向

    // 欧拉角
    float Yaw;               // 偏航角
    float Pitch;             // 俯仰角

    // 相机选项
    float MovementSpeed;     // 移动速度
    float MouseSensitivity;  // 鼠标灵敏度
    float Zoom;              // 视野角度（用于透视投影）

    // 构造函数：使用向量
    Camera(Vector3 position = Vector3(0.0f, 0.0f, 3.0f),
           Vector3 up = Vector3(0.0f, 1.0f, 0.0f),
           float yaw = YAW,
           float pitch = PITCH);

    // 构造函数：使用标量
    Camera(float posX, float posY, float posZ,
           float upX, float upY, float upZ,
           float yaw, float pitch);

    // 获取视图矩阵
    Matrix4 GetViewMatrix() const;

    // 处理键盘输入
    void ProcessKeyboard(CameraMovement direction, float deltaTime);

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // 处理鼠标滚轮
    void ProcessMouseScroll(float yoffset);

private:
    // 更新相机向量（从欧拉角计算前、右、上向量）
    void UpdateCameraVectors();
};

#endif // CAMERA_H
