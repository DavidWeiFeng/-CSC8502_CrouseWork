#version 330 core

// 输出：片段颜色
out vec4 FragColor;

// 输入：从顶点着色器传来的纹理坐标（3D方向向量）
in vec3 TexCoords;

// 立方体贴图采样器
// 注意：这是 samplerCube 类型，不是普通的 sampler2D
uniform samplerCube skybox;

void main()
{
    // 使用3D方向向量采样立方体贴图
    // texture() 函数会自动确定应该采样哪个面（+X, -X, +Y, -Y, +Z, -Z）
    // 并在该面上进行纹理采样
    FragColor = texture(skybox, TexCoords);
}

/*
 * 立方体贴图采样原理：
 *
 * 1. 什么是 samplerCube？
 *    - samplerCube 是专门用于立方体贴图的采样器类型
 *    - 与 sampler2D 不同，它接受 vec3（3D方向向量）作为纹理坐标
 *
 * 2. 如何确定采样哪个面？
 *    - OpenGL 根据方向向量中绝对值最大的分量确定面
 *    - 例如：
 *      * (2.0, 0.5, 0.3) → |x|最大且为正 → 采样 +X 面（右）
 *      * (-1.5, 0.8, 0.2) → |x|最大且为负 → 采样 -X 面（左）
 *      * (0.3, 3.0, 0.5) → |y|最大且为正 → 采样 +Y 面（上）
 *
 * 3. 确定面内的具体位置
 *    - 选定面后，OpenGL 使用另外两个分量计算该面上的 2D 坐标
 *    - 例如对于 +X 面，使用 z 和 y 分量计算 (u, v) 坐标
 *
 * 4. 为什么天空盒看起来是连续的？
 *    - 6个面的纹理在边缘处是连续匹配的
 *    - 使用 GL_CLAMP_TO_EDGE 防止采样到对面纹理
 *    - 这确保了立方体看起来是无缝的
 */
