#include "WaterPlane.h"
#include "Shader.h"
#include <iostream>

// ========================================
// 构造函数 - 创建水平面对象
// ========================================
WaterPlane::WaterPlane(float waterLevel, float size, int resolution)
    : m_VAO(0)
    , m_VBO(0)
    , m_EBO(0)
    , m_WaterLevel(waterLevel)
    , m_Size(size)
    , m_Resolution(resolution)
    , m_IndexCount(0)
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "开始创建水面..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "  水面高度: Y = " << m_WaterLevel << std::endl;
    std::cout << "  水面大小: " << m_Size << " x " << m_Size << std::endl;
    std::cout << "  网格分辨率: " << m_Resolution << " x " << m_Resolution << std::endl;

    // ========================================
    // 步骤1：生成网格数据
    // ========================================
    std::cout << "\n[步骤1] 生成水面网格..." << std::endl;
    GenerateMesh();
    std::cout << "✓ 生成了 " << m_Vertices.size() << " 个顶点" << std::endl;
    std::cout << "✓ 生成了 " << m_Indices.size() / 3 << " 个三角形" << std::endl;

    // ========================================
    // 步骤2：设置OpenGL缓冲对象
    // ========================================
    std::cout << "\n[步骤2] 创建GPU缓冲对象..." << std::endl;
    SetupMesh();
    std::cout << "✓ VAO/VBO/EBO创建成功" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "水面创建完成！" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// ========================================
// 析构函数 - 清理GPU资源
// ========================================
WaterPlane::~WaterPlane()
{
    // 删除OpenGL对象，释放GPU内存
    if (m_VAO != 0)
        glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0)
        glDeleteBuffers(1, &m_EBO);

    std::cout << "水面对象已销毁，GPU资源已释放" << std::endl;
}

// ========================================
// 生成水面网格
// ========================================
// 工作原理：
// 创建一个resolution x resolution的平面网格
// 所有顶点初始Y坐标都是waterLevel
// 波浪动画在着色器中实现（通过顶点着色器的time uniform）
// ========================================
void WaterPlane::GenerateMesh()
{
    // ========================================
    // 步骤1：生成顶点
    // ========================================
    // 预分配内存
    int totalVertices = m_Resolution * m_Resolution;
    m_Vertices.resize(totalVertices);

    // 计算每个网格单元的大小
    float cellSize = m_Size / (m_Resolution - 1);

    // 计算起始偏移（让水面中心在原点）
    float startX = -m_Size / 2.0f;
    float startZ = -m_Size / 2.0f;

    // 遍历每个顶点
    for (int z = 0; z < m_Resolution; ++z)
    {
        for (int x = 0; x < m_Resolution; ++x)
        {
            int index = z * m_Resolution + x;

            // ========================================
            // 设置顶点位置
            // ========================================
            // X和Z均匀分布在水面上
            // Y固定为waterLevel（波浪在着色器中动态计算）
            float posX = startX + x * cellSize;
            float posY = m_WaterLevel;
            float posZ = startZ + z * cellSize;
            m_Vertices[index].Position = glm::vec3(posX, posY, posZ);

            // ========================================
            // 设置法向量（初始向上）
            // ========================================
            // 平静水面的法向量指向正上方
            // 波浪的法向量会在着色器中动态计算
            m_Vertices[index].Normal = glm::vec3(0.0f, 1.0f, 0.0f);

            // ========================================
            // 设置纹理坐标
            // ========================================
            // 范围 0.0 到 1.0
            // 用于采样水纹理或作为波浪计算的输入
            float u = static_cast<float>(x) / (m_Resolution - 1);
            float v = static_cast<float>(z) / (m_Resolution - 1);
            m_Vertices[index].TexCoord = glm::vec2(u, v);
        }
    }

    // ========================================
    // 步骤2：生成三角形索引
    // ========================================
    // 与地形相同的拓扑结构：每个格子2个三角形
    int numQuads = (m_Resolution - 1) * (m_Resolution - 1);
    m_Indices.reserve(numQuads * 6);

    for (int z = 0; z < m_Resolution - 1; ++z)
    {
        for (int x = 0; x < m_Resolution - 1; ++x)
        {
            // 当前格子的4个顶点索引
            //   topLeft ─── topRight
            //      │           │
            //   bottomLeft ─ bottomRight
            int topLeft     = z * m_Resolution + x;
            int topRight    = z * m_Resolution + (x + 1);
            int bottomLeft  = (z + 1) * m_Resolution + x;
            int bottomRight = (z + 1) * m_Resolution + (x + 1);

            // 三角形1（逆时针）
            m_Indices.push_back(topLeft);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(topRight);

            // 三角形2（逆时针）
            m_Indices.push_back(topRight);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(bottomRight);
        }
    }

    m_IndexCount = static_cast<unsigned int>(m_Indices.size());
}

// ========================================
// 设置OpenGL网格缓冲
// ========================================
// 配置VAO、VBO、EBO和顶点属性
// ========================================
void WaterPlane::SetupMesh()
{
    // ========================================
    // 步骤1：生成并绑定VAO
    // ========================================
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // ========================================
    // 步骤2：创建VBO，上传顶点数据
    // ========================================
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 m_Vertices.size() * sizeof(Vertex),
                 m_Vertices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤3：创建EBO，上传索引数据
    // ========================================
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_Indices.size() * sizeof(unsigned int),
                 m_Indices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤4：配置顶点属性指针
    // ========================================
    // Vertex结构体内存布局：
    // [Position(12字节), Normal(12字节), TexCoord(8字节)] = 32字节

    // 属性0：顶点位置 (vec3)
    glVertexAttribPointer(
        0,                              // location = 0
        3,                              // 3个分量 (x, y, z)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)0                        // 偏移量0
    );
    glEnableVertexAttribArray(0);

    // 属性1：法向量 (vec3)
    glVertexAttribPointer(
        1,                              // location = 1
        3,                              // 3个分量 (x, y, z)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal) // 偏移量12字节
    );
    glEnableVertexAttribArray(1);

    // 属性2：纹理坐标 (vec2)
    glVertexAttribPointer(
        2,                              // location = 2
        2,                              // 2个分量 (u, v)
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoord) // 偏移量24字节
    );
    glEnableVertexAttribArray(2);

    // ========================================
    // 步骤5：解绑VAO
    // ========================================
    glBindVertexArray(0);

    std::cout << "  - VAO ID: " << m_VAO << std::endl;
    std::cout << "  - VBO ID: " << m_VBO << " (存储 " << m_Vertices.size() << " 个顶点)" << std::endl;
    std::cout << "  - EBO ID: " << m_EBO << " (存储 " << m_Indices.size() << " 个索引)" << std::endl;
}

// ========================================
// 渲染水面
// ========================================
// 注意：
// - 调用前必须激活着色器并设置所有uniform
// - 调用前必须绑定天空盒立方体贴图纹理
// - 建议启用混合模式以实现半透明效果
// ========================================
void WaterPlane::Render()
{
    // 绑定VAO（恢复顶点属性配置）
    glBindVertexArray(m_VAO);

    // 使用索引绘制三角形
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);

    // 解绑VAO
    glBindVertexArray(0);
}
