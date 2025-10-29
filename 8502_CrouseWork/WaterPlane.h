#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

// 前向声明
class Shader;

/**
 * @class WaterPlane
 * @brief 水平面类 - 渲染带有反射效果的动态水面
 *
 * 功能特性：
 * - 生成细分网格的水平面（用于波浪效果）
 * - 使用天空盒立方体贴图实现水面反射
 * - 支持动态波浪动画（正弦波叠加）
 * - 菲涅尔效应（视角不同，反射/透明度不同）
 * - 半透明渲染（支持Alpha混合）
 *
 * 课程要求：
 * - 满足"环境映射演示"要求（使用天空盒反射）
 * - 纹理映射（采样立方体贴图）
 * - 视觉效果（动态波浪）
 */
class WaterPlane {
public:
    /**
     * @brief 构造函数 - 创建水平面网格
     * @param waterLevel 水面高度（世界坐标Y值，建议0.0）
     * @param size 水面大小（正方形边长，如100.0f）
     * @param resolution 网格分辨率（每边顶点数，如100）
     *                   分辨率越高，波浪越平滑，但性能开销越大
     *                   建议范围：50-200
     */
    WaterPlane(float waterLevel, float size, int resolution);

    /**
     * @brief 析构函数 - 释放OpenGL资源
     */
    ~WaterPlane();

    /**
     * @brief 渲染水面
     * @param shader 水面着色器
     *
     * 注意：调用前必须：
     * 1. 激活着色器 shader.Use()
     * 2. 设置所有uniform（model, view, projection, time, viewPos等）
     * 3. 绑定天空盒立方体贴图纹理
     * 4. 启用混合模式 glEnable(GL_BLEND)
     */
    void Render();

    /**
     * @brief 获取水面高度
     * @return 水面的Y坐标
     */
    float GetWaterLevel() const { return m_WaterLevel; }

private:
    // OpenGL对象
    unsigned int m_VAO;         // 顶点数组对象
    unsigned int m_VBO;         // 顶点缓冲对象
    unsigned int m_EBO;         // 索引缓冲对象

    // 水面参数
    float m_WaterLevel;         // 水面高度（Y坐标）
    float m_Size;               // 水面大小（边长）
    int m_Resolution;           // 网格分辨率（每边顶点数）
    unsigned int m_IndexCount;  // 索引数量（用于渲染）

    // 顶点结构体
    struct Vertex {
        glm::vec3 Position;     // 顶点位置
        glm::vec3 Normal;       // 法向量（用于光照和反射计算）
        glm::vec2 TexCoord;     // 纹理坐标
    };

    std::vector<Vertex> m_Vertices;         // 顶点数组
    std::vector<unsigned int> m_Indices;    // 索引数组

    /**
     * @brief 生成水面网格数据
     *
     * 工作原理：
     * 1. 创建resolution x resolution的网格
     * 2. 所有顶点Y坐标初始为waterLevel
     * 3. 法向量初始指向上方(0,1,0)
     * 4. 波浪动画在着色器中实现（通过time uniform）
     */
    void GenerateMesh();

    /**
     * @brief 设置OpenGL缓冲对象
     *
     * 配置VAO/VBO/EBO，并设置顶点属性指针：
     * - Location 0: 位置 (vec3)
     * - Location 1: 法向量 (vec3)
     * - Location 2: 纹理坐标 (vec2)
     */
    void SetupMesh();
};
