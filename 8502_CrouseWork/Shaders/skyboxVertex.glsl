#version 330 core

// 输入：顶点位置
layout (location = 0) in vec3 aPos;

// 输出到片段着色器：纹理坐标（实际上是3D方向向量）
out vec3 TexCoords;

// Uniform变量
uniform mat4 projection;  // 投影矩阵
uniform mat4 view;        // 视图矩阵

void main()
{
    // 直接使用顶点位置作为纹理坐标（方向向量）
    // 这是立方体贴图的特点：使用3D方向向量采样
    TexCoords = aPos;

    // 关键技巧1：去除view矩阵的平移分量
    // mat4(mat3(view)) 会将4x4矩阵转为3x3（去除平移），再转回4x4
    // 这样天空盒只会跟随相机旋转，不会跟随相机平移（保持无限远的感觉）
    mat4 viewWithoutTranslation = mat4(mat3(view));

    // 计算顶点的裁剪空间位置
    vec4 pos = projection * viewWithoutTranslation * vec4(aPos, 1.0);

    // 关键技巧2：设置深度值为1.0（最远）
    // 通过设置 gl_Position.z = gl_Position.w
    // 在透视除法后，深度值 = z/w = w/w = 1.0（最大深度）
    // 这确保天空盒永远渲染在所有物体的后面
    gl_Position = pos.xyww;
}

/*
 * 天空盒渲染原理说明：
 *
 * 1. 为什么使用 mat4(mat3(view))？
 *    - 相机在真实世界中移动时，远处的天空看起来是不动的
 *    - 通过去除平移分量，只保留旋转，天空盒跟随视角旋转但不移动
 *    - 这创造了"无限远"的视觉效果
 *
 * 2. 为什么使用 xyww 而不是 xyzw？
 *    - 正常情况：gl_Position = (x, y, z, w)，深度 = z/w
 *    - 天空盒：gl_Position = (x, y, w, w)，深度 = w/w = 1.0
 *    - 深度值1.0是深度缓冲的最大值，表示最远距离
 *    - 配合 glDepthFunc(GL_LEQUAL) 允许深度值为1.0通过测试
 *
 * 3. 为什么顶点位置可以直接作为纹理坐标？
 *    - 立方体贴图使用3D方向向量而不是2D (u,v) 坐标
 *    - 从原点指向顶点的方向正好对应该顶点应该采样的纹理位置
 *    - 例如：(1, 0, 0) 采样 +X 面（右），(-1, 0, 0) 采样 -X 面（左）
 */
