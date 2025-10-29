#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>

// 前向声明
class Shader;

/**
 * @class Water
 * @brief 水面类 - 渲染平面水面，展示强镜面反射效果
 *
 * 功能：
 * - 创建平面水面（2个三角形组成的矩形）
 * - 放置在地形上方的固定高度
 * - 使用强镜面反射参数展示高光效果
 * - 支持半透明效果（可选）
 */
class Water {
public:
    /**
     * @brief 构造函数 - 创建水面
     * @param position 水面中心位置（世界坐标）
     * @param size 水面大小（宽度和深度）
     */
    Water(const glm::vec3& position, float size);

    /**
     * @brief 析构函数 - 释放OpenGL资源
     */
    ~Water();

    /**
     * @brief 渲染水面
     */
    void Render();

    /**
     * @brief 获取水面高度（Y坐标）
     */
    float GetHeight() const { return position.y; }

    /**
     * @brief 设置水面高度
     */
    void SetHeight(float height) { position.y = height; }

private:
    GLuint VAO;              // 顶点数组对象
    GLuint VBO;              // 顶点缓冲对象
    GLuint EBO;              // 索引缓冲对象

    glm::vec3 position;      // 水面位置
    float size;              // 水面大小

    /**
     * @brief 初始化水面平面的顶点数据
     */
    void setupWater();
};
