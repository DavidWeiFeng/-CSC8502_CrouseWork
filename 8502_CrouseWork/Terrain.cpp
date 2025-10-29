#include "Terrain.h"
#include <iostream>
#include <algorithm>

// ========================================
// STB_IMAGE - 图像加载库
// ========================================
// 注意：STB_IMAGE_IMPLEMENTATION 已经在 Texture.cpp 中定义
// 这里只需要包含头文件即可
#include "stb_image.h"

// ========================================
// 构造函数 - 创建地形对象
// ========================================
Terrain::Terrain(const std::string& heightmapPath,
                 float terrainSize,
                 float heightScale)
    : m_VAO(0)              // 初始化VAO为0（未创建）
    , m_VBO(0)              // 初始化VBO为0
    , m_EBO(0)              // 初始化EBO为0
    , m_Width(0)            // 高度图宽度，稍后加载时设置
    , m_Height(0)           // 高度图高度，稍后加载时设置
    , m_TerrainSize(terrainSize)    // 存储地形大小参数
    , m_HeightScale(heightScale)    // 存储高度缩放参数
    , m_IndexCount(0)       // 索引数量，稍后生成索引时设置
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "开始创建地形..." << std::endl;
    std::cout << "========================================" << std::endl;

    // ========================================
    // 步骤1：加载高度图
    // ========================================
    std::cout << "\n[步骤1] 加载高度图: " << heightmapPath << std::endl;
    if (!LoadHeightmap(heightmapPath))
    {
        std::cerr << "错误：无法加载高度图！" << std::endl;
        return;
    }
    std::cout << "✓ 成功加载高度图 (" << m_Width << " x " << m_Height << ")" << std::endl;

    // ========================================
    // 步骤2：生成顶点数据
    // ========================================
    std::cout << "\n[步骤2] 生成地形顶点..." << std::endl;
    GenerateVertices();
    std::cout << "✓ 生成了 " << m_Vertices.size() << " 个顶点" << std::endl;

    // ========================================
    // 步骤3：生成三角形索引
    // ========================================
    std::cout << "\n[步骤3] 生成三角形索引..." << std::endl;
    GenerateIndices();
    std::cout << "✓ 生成了 " << m_Indices.size() / 3 << " 个三角形 ("
              << m_Indices.size() << " 个索引)" << std::endl;

    // ========================================
    // 步骤4：计算法向量
    // ========================================
    std::cout << "\n[步骤4] 计算顶点法向量..." << std::endl;
    CalculateNormals();
    std::cout << "✓ 法向量计算完成" << std::endl;

    // ========================================
    // 步骤5：设置OpenGL缓冲对象
    // ========================================
    std::cout << "\n[步骤5] 创建OpenGL缓冲对象..." << std::endl;
    SetupMesh();
    std::cout << "✓ GPU缓冲对象创建成功" << std::endl;

    std::cout << "\n========================================" << std::endl;
    std::cout << "地形创建完成！" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

// ========================================
// 析构函数 - 清理资源
// ========================================
Terrain::~Terrain()
{
    // 删除OpenGL缓冲对象，释放GPU内存
    // 注意：必须按顺序删除，先删VAO再删VBO/EBO
    if (m_VAO != 0)
        glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO != 0)
        glDeleteBuffers(1, &m_VBO);
    if (m_EBO != 0)
        glDeleteBuffers(1, &m_EBO);

    std::cout << "地形对象已销毁，GPU资源已释放" << std::endl;
}

// ========================================
// 加载高度图
// ========================================
// 工作原理：
// 1. 使用STB_image读取图像文件
// 2. 提取每个像素的亮度值（0-255）
// 3. 存储到 m_HeightData 数组中
// ========================================
bool Terrain::LoadHeightmap(const std::string& path)
{
    // ========================================
    // 使用STB_image加载图像
    // ========================================
    // 参数：
    //   path.c_str()  - 文件路径（C风格字符串）
    //   &m_Width      - 输出：图像宽度
    //   &m_Height     - 输出：图像高度
    //   &channels     - 输出：颜色通道数（1=灰度，3=RGB，4=RGBA）
    //   1             - 我们要求：强制转为1通道（灰度图）
    // ========================================
    int channels;
    unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &channels, 1);

    // 检查是否加载成功
    if (!data)
    {
        std::cerr << "错误：无法加载图像 " << path << std::endl;
        std::cerr << "STB错误信息: " << stbi_failure_reason() << std::endl;
        return false;
    }

    // ========================================
    // 将图像数据转换为高度值
    // ========================================
    // m_HeightData 存储每个像素的高度（浮点数0.0-1.0）
    m_HeightData.resize(m_Width * m_Height);

    for (int z = 0; z < m_Height; ++z)          // 遍历每一行（Z轴）
    {
        for (int x = 0; x < m_Width; ++x)       // 遍历每一列（X轴）
        {
            // 计算像素在一维数组中的索引
            // 公式：index = z * width + x
            int index = z * m_Width + x;

            // 将像素值（0-255）归一化到 0.0-1.0 范围
            // 除以255.0f得到0到1之间的浮点数
            m_HeightData[index] = static_cast<float>(data[index]) / 255.0f;
        }
    }

    // ========================================
    // 释放图像数据内存
    // ========================================
    // STB_image分配的内存需要用stbi_image_free释放
    stbi_image_free(data);

    return true;
}

// ========================================
// 生成地形顶点
// ========================================
// 工作原理：
// 为高度图的每个像素创建一个3D顶点
//
// 例如：3x3的高度图会生成9个顶点：
//   (0,h,0) (1,h,0) (2,h,0)
//   (0,h,1) (1,h,1) (2,h,1)
//   (0,h,2) (1,h,2) (2,h,2)
// ========================================
void Terrain::GenerateVertices()
{
    // 预分配内存，提高效率
    // 顶点数 = 宽度 × 高度
    m_Vertices.resize(m_Width * m_Height);

    // 计算每个网格单元的实际大小
    // 如果地形大小是100，分辨率是256，则每格大小 = 100/255 ≈ 0.39
    float cellSizeX = m_TerrainSize / (m_Width - 1);
    float cellSizeZ = m_TerrainSize / (m_Height - 1);

    // ========================================
    // 遍历每个高度图像素，生成对应的3D顶点
    // ========================================
    for (int z = 0; z < m_Height; ++z)          // Z轴（深度方向）
    {
        for (int x = 0; x < m_Width; ++x)       // X轴（宽度方向）
        {
            // 获取当前顶点在数组中的索引
            int vertexIndex = GetVertexIndex(x, z);

            // ========================================
            // 1. 设置顶点位置
            // ========================================
            // X坐标：从0到terrainSize，均匀分布
            float posX = x * cellSizeX;

            // Y坐标（高度）：从高度图读取，乘以缩放系数
            // 不使用偏移量，让地形从Y=0开始
            float posY = GetHeight(x, z) * m_HeightScale;

            // Z坐标：从0到terrainSize，均匀分布
            float posZ = z * cellSizeZ;

            m_Vertices[vertexIndex].Position = glm::vec3(posX, posY, posZ);

            // ========================================
            // 2. 设置纹理坐标
            // ========================================
            // 纹理坐标范围：0.0 到 1.0
            // u = x / (宽度-1)，从左到右：0 → 1
            // v = z / (高度-1)，从前到后：0 → 1
            float u = static_cast<float>(x) / (m_Width - 1);
            float v = static_cast<float>(z) / (m_Height - 1);
            m_Vertices[vertexIndex].TexCoord = glm::vec2(u, v);

            // ========================================
            // 3. 法向量初始化为(0,1,0)
            // ========================================
            // 先设置为向上的向量，稍后会重新计算
            m_Vertices[vertexIndex].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

// ========================================
// 生成三角形索引
// ========================================
// 工作原理：
// 将网格顶点连接成三角形
//
// 每个格子分成2个三角形：
//   v0 ─── v1       三角形1: [v0, v1, v2]
//   │  ╲   │        三角形2: [v1, v3, v2]
//   │    ╲ │
//   v2 ─── v3
// ========================================
void Terrain::GenerateIndices()
{
    // 计算需要的索引数量
    // (宽度-1) × (高度-1) 个格子
    // 每个格子 = 2个三角形 = 6个索引
    int numQuads = (m_Width - 1) * (m_Height - 1);
    m_Indices.reserve(numQuads * 6);

    // ========================================
    // 遍历每个格子，生成2个三角形
    // ========================================
    for (int z = 0; z < m_Height - 1; ++z)      // 不包括最后一行
    {
        for (int x = 0; x < m_Width - 1; ++x)   // 不包括最后一列
        {
            // ========================================
            // 获取当前格子的4个顶点索引
            // ========================================
            //   topLeft ─── topRight
            //      │           │
            //   bottomLeft ─ bottomRight
            // ========================================
            int topLeft     = GetVertexIndex(x, z);
            int topRight    = GetVertexIndex(x + 1, z);
            int bottomLeft  = GetVertexIndex(x, z + 1);
            int bottomRight = GetVertexIndex(x + 1, z + 1);

            // ========================================
            // 三角形1（左上三角形）
            // ========================================
            // 顶点顺序：逆时针（OpenGL默认正面）
            m_Indices.push_back(topLeft);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(topRight);

            // ========================================
            // 三角形2（右下三角形）
            // ========================================
            m_Indices.push_back(topRight);
            m_Indices.push_back(bottomLeft);
            m_Indices.push_back(bottomRight);
        }
    }

    // 存储索引总数（Render时需要用到）
    m_IndexCount = static_cast<unsigned int>(m_Indices.size());
}

// ========================================
// 计算法向量
// ========================================
// 工作原理：
// 1. 对于每个三角形，计算其法向量（使用叉乘）
// 2. 将三角形法向量累加到其3个顶点上
// 3. 最后归一化每个顶点的法向量
//
// 为什么这样做？
// - 平滑着色需要每个顶点的法向量
// - 通过平均周围三角形的法向量，得到平滑效果
// ========================================
void Terrain::CalculateNormals()
{
    // ========================================
    // 步骤1：先将所有法向量清零
    // ========================================
    for (auto& vertex : m_Vertices)
    {
        vertex.Normal = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    // ========================================
    // 步骤2：遍历每个三角形，计算并累加法向量
    // ========================================
    // 每3个索引定义一个三角形
    for (size_t i = 0; i < m_Indices.size(); i += 3)
    {
        // 获取三角形的3个顶点索引
        unsigned int index0 = m_Indices[i];
        unsigned int index1 = m_Indices[i + 1];
        unsigned int index2 = m_Indices[i + 2];

        // 获取3个顶点的位置
        glm::vec3 v0 = m_Vertices[index0].Position;
        glm::vec3 v1 = m_Vertices[index1].Position;
        glm::vec3 v2 = m_Vertices[index2].Position;

        // ========================================
        // 计算三角形的两条边向量
        // ========================================
        //      v1
        //      /\
        //     /  \
        //    /    \
        //   v0 ──── v2
        //
        // edge1 = v1 - v0（从v0指向v1）
        // edge2 = v2 - v0（从v0指向v2）
        // ========================================
        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;

        // ========================================
        // 使用叉乘计算法向量
        // ========================================
        // cross(edge1, edge2) 得到垂直于三角形平面的向量
        // 这个向量的方向遵循右手定则
        // ========================================
        glm::vec3 normal = glm::cross(edge1, edge2);

        // ========================================
        // 将法向量累加到3个顶点上
        // ========================================
        // 因为一个顶点可能被多个三角形共享
        // 所以我们累加所有相关三角形的法向量
        // ========================================
        m_Vertices[index0].Normal += normal;
        m_Vertices[index1].Normal += normal;
        m_Vertices[index2].Normal += normal;
    }

    // ========================================
    // 步骤3：归一化所有法向量
    // ========================================
    // 归一化 = 将向量长度变为1，但保持方向不变
    // 这对于光照计算很重要
    // ========================================
    for (auto& vertex : m_Vertices)
    {
        vertex.Normal = glm::normalize(vertex.Normal);
    }
}

// ========================================
// 设置OpenGL网格缓冲
// ========================================
// 工作原理：
// 1. 创建VAO（顶点数组对象）
// 2. 创建VBO（顶点缓冲对象），存储顶点数据
// 3. 创建EBO（索引缓冲对象），存储索引数据
// 4. 配置顶点属性指针（告诉OpenGL如何解释数据）
// ========================================
void Terrain::SetupMesh()
{
    // ========================================
    // 步骤1：生成并绑定VAO
    // ========================================
    // VAO会记住我们接下来的所有配置
    // 以后只需绑定这个VAO，OpenGL就知道如何读取数据
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    // ========================================
    // 步骤2：创建VBO，传输顶点数据到GPU
    // ========================================
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    // 将顶点数据复制到VBO
    // 参数：
    //   GL_ARRAY_BUFFER - 目标缓冲类型
    //   大小 = 顶点数 × 每个顶点的字节数
    //   数据指针 = m_Vertices数组的首地址
    //   GL_STATIC_DRAW - 数据不会改变（静态地形）
    glBufferData(GL_ARRAY_BUFFER,
                 m_Vertices.size() * sizeof(Vertex),
                 m_Vertices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤3：创建EBO，传输索引数据到GPU
    // ========================================
    glGenBuffers(1, &m_EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);

    // 将索引数据复制到EBO
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 m_Indices.size() * sizeof(unsigned int),
                 m_Indices.data(),
                 GL_STATIC_DRAW);

    // ========================================
    // 步骤4：配置顶点属性指针
    // ========================================
    // 告诉OpenGL如何解释VBO中的数据
    //
    // Vertex结构体的内存布局：
    // [Position.x, Position.y, Position.z,     ← 12字节
    //  Normal.x, Normal.y, Normal.z,           ← 12字节
    //  TexCoord.x, TexCoord.y]                 ← 8字节
    // 总共：32字节
    // ========================================

    // ────────────────────────────────────────
    // 属性0：顶点位置（vec3）
    // ────────────────────────────────────────
    glVertexAttribPointer(
        0,                              // location = 0（对应着色器中的 layout(location=0)）
        3,                              // 每个属性有3个分量（x, y, z）
        GL_FLOAT,                       // 数据类型是float
        GL_FALSE,                       // 不需要归一化
        sizeof(Vertex),                 // 步长 = 每个顶点的总字节数（32字节）
        (void*)0                        // 偏移量 = 0（Position在最开始）
    );
    glEnableVertexAttribArray(0);       // 启用属性0

    // ────────────────────────────────────────
    // 属性1：顶点法向量（vec3）
    // ────────────────────────────────────────
    glVertexAttribPointer(
        1,                              // location = 1
        3,                              // 3个分量（x, y, z）
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, Normal) // 偏移量 = Normal在结构体中的位置（12字节）
    );
    glEnableVertexAttribArray(1);

    // ────────────────────────────────────────
    // 属性2：纹理坐标（vec2）
    // ────────────────────────────────────────
    glVertexAttribPointer(
        2,                              // location = 2
        2,                              // 2个分量（u, v）
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void*)offsetof(Vertex, TexCoord) // 偏移量 = TexCoord的位置（24字节）
    );
    glEnableVertexAttribArray(2);

    // ========================================
    // 步骤5：解绑VAO（保护配置）
    // ========================================
    // 解绑后，后续的OpenGL调用不会影响这个VAO的配置
    glBindVertexArray(0);

    std::cout << "  - VAO ID: " << m_VAO << std::endl;
    std::cout << "  - VBO ID: " << m_VBO << " (存储 " << m_Vertices.size() << " 个顶点)" << std::endl;
    std::cout << "  - EBO ID: " << m_EBO << " (存储 " << m_Indices.size() << " 个索引)" << std::endl;
}

// ========================================
// 渲染地形
// ========================================
// 注意：
// - 调用前必须先 shader.Use()
// - 调用前必须设置所有uniform变量（model, view, projection等）
// ========================================
void Terrain::Render()
{
    // 绑定VAO
    // 这会恢复我们之前配置的所有顶点属性
    glBindVertexArray(m_VAO);

    // ========================================
    // 使用索引绘制三角形
    // ========================================
    // glDrawElements - 使用索引缓冲绘制
    // 参数：
    //   GL_TRIANGLES - 绘制三角形
    //   m_IndexCount - 索引数量
    //   GL_UNSIGNED_INT - 索引类型
    //   0 - 索引数组的偏移量
    // ========================================
    glDrawElements(GL_TRIANGLES, m_IndexCount, GL_UNSIGNED_INT, 0);

    // 解绑VAO（良好习惯）
    glBindVertexArray(0);
}

// ========================================
// 辅助函数：获取指定网格位置的高度
// ========================================
float Terrain::GetHeight(int x, int z) const
{
    // 边界检查：确保坐标在有效范围内
    if (x < 0 || x >= m_Width || z < 0 || z >= m_Height)
        return 0.0f;

    // 返回高度图数据（0.0 - 1.0）
    return m_HeightData[z * m_Width + x];
}

// ========================================
// 辅助函数：获取顶点索引
// ========================================
int Terrain::GetVertexIndex(int x, int z) const
{
    // 将2D坐标(x, z)转换为1D数组索引
    // 公式：index = z * width + x
    return z * m_Width + x;
}

// ========================================
// 获取世界坐标位置的地形高度
// ========================================
// 用途：在地形上放置物体（树、岩石等）
// 例如：float y = terrain.GetHeightAt(10.5f, 20.3f);
// ========================================
float Terrain::GetHeightAt(float worldX, float worldZ) const
{
    // ========================================
    // 步骤1：将世界坐标转换为网格坐标
    // ========================================
    float gridX = (worldX / m_TerrainSize) * (m_Width - 1);
    float gridZ = (worldZ / m_TerrainSize) * (m_Height - 1);

    // ========================================
    // 步骤2：获取所在格子的左上角整数坐标
    // ========================================
    int x0 = static_cast<int>(gridX);
    int z0 = static_cast<int>(gridZ);

    // 边界检查
    if (x0 < 0 || x0 >= m_Width - 1 || z0 < 0 || z0 >= m_Height - 1)
        return 0.0f;

    // ========================================
    // 步骤3：双线性插值
    // ========================================
    // 获取格子4个角的高度
    float h00 = GetHeight(x0, z0);          // 左上
    float h10 = GetHeight(x0 + 1, z0);      // 右上
    float h01 = GetHeight(x0, z0 + 1);      // 左下
    float h11 = GetHeight(x0 + 1, z0 + 1);  // 右下

    // 计算在格子内的相对位置（0.0 - 1.0）
    float tx = gridX - x0;
    float tz = gridZ - z0;

    // 双线性插值公式
    float h0 = h00 * (1 - tx) + h10 * tx;   // 上边插值
    float h1 = h01 * (1 - tx) + h11 * tx;   // 下边插值
    float finalHeight = h0 * (1 - tz) + h1 * tz;

    // 应用高度缩放
    return finalHeight * m_HeightScale;
}
