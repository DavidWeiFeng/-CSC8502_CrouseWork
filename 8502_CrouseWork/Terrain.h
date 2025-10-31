#ifndef TERRAIN_H
#define TERRAIN_H

#include <glad/glad.h>
#include "nclgl/Vector2.h"
#include "nclgl/Vector3.h"
#include <string>
#include <vector>

// ========================================
// 地形类 - 从高度图生成3D地形
// ========================================
// 功能：
// 1. 加载高度图图像（灰度图）
// 2. 根据像素亮度生成地形网格
// 3. 计算法向量（用于光照）
// 4. 渲染地形
// ========================================

class Terrain
{
public:
    // ========================================
    // 构造函数
    // ========================================
    // 参数：
    //   heightmapPath - 高度图文件路径（如 "Textures/heightmap.png"）
    //   terrainSize   - 地形在世界坐标中的实际大小（默认100.0单位）
    //   heightScale   - 高度缩放系数（默认10.0，数值越大山越高）
    // ========================================
    Terrain(const std::string& heightmapPath,
            float terrainSize = 100.0f,
            float heightScale = 10.0f);

    // ========================================
    // 析构函数 - 清理OpenGL资源
    // ========================================
    ~Terrain();

    // ========================================
    // 渲染地形
    // ========================================
    // 注意：调用前必须先激活着色器并设置uniform变量
    // ========================================
    void Render();

    // ========================================
    // 获取地形宽度和深度（网格分辨率）
    // ========================================
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    // ========================================
    // 获取指定位置的地形高度（用于放置物体）
    // ========================================
    // 参数：
    //   worldX - 世界坐标X
    //   worldZ - 世界坐标Z
    // 返回：该位置的Y高度值
    // ========================================
    float GetHeightAt(float worldX, float worldZ) const;

private:
    // ========================================
    // 顶点结构体 - 定义每个顶点的数据
    // ========================================
    struct Vertex
    {
        Vector3 Position;   // 顶点位置 (x, y, z)
        Vector3 Normal;     // 法向量（用于光照计算）
        Vector2 TexCoord;   // 纹理坐标 (u, v)
    };

    // ========================================
    // OpenGL对象ID
    // ========================================
    GLuint m_VAO;              // 顶点数组对象 - 记录顶点属性配置
    GLuint m_VBO;              // 顶点缓冲对象 - 存储顶点数据
    GLuint m_EBO;              // 索引缓冲对象 - 存储三角形索引

    // ========================================
    // 地形数据
    // ========================================
    std::vector<Vertex> m_Vertices;        // 所有顶点数据
    std::vector<unsigned int> m_Indices;   // 三角形索引
    std::vector<float> m_HeightData;       // 高度图数据（存储每个像素的高度值）

    // ========================================
    // 地形参数
    // ========================================
    int m_Width;               // 高度图宽度（像素）
    int m_Height;              // 高度图高度（像素）
    float m_TerrainSize;       // 地形实际大小（世界坐标）
    float m_HeightScale;       // 高度缩放系数
    unsigned int m_IndexCount; // 总索引数量（用于渲染时的drawElements）

    // ========================================
    // 私有函数 - 地形生成流程
    // ========================================

    // 1. 加载高度图图像
    // 使用STB_image读取图像，提取每个像素的亮度值
    bool LoadHeightmap(const std::string& path);

    // 2. 生成网格顶点
    // 为每个高度图像素创建一个3D顶点
    void GenerateVertices();

    // 3. 生成三角形索引
    // 将顶点连接成三角形网格
    void GenerateIndices();

    // 4. 计算法向量
    // 为每个顶点计算平滑法向量（用于光照）
    void CalculateNormals();

    // 5. 设置OpenGL缓冲对象
    // 将顶点和索引数据传给GPU
    void SetupMesh();

    // ========================================
    // 辅助函数
    // ========================================

    // 获取指定网格坐标(x,z)的高度值
    float GetHeight(int x, int z) const;

    // 获取指定网格坐标的顶点索引
    int GetVertexIndex(int x, int z) const;
};

#endif // TERRAIN_H
