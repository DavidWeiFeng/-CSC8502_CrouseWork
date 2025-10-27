#version 330 core

// 顶点着色器 - 处理每个顶点

// 输入：顶点属性
layout(location = 0) in vec3 position;  // 顶点位置
layout(location = 1) in vec3 color;     // 顶点颜色

// 输出：传递到片段着色器的数据
out vec3 fragColor;  // 传递颜色到片段着色器

// Uniform变量：从C++程序传入的变量
uniform mat4 model;       // 模型矩阵：物体在世界中的位置/旋转/缩放
uniform mat4 view;        // 视图矩阵：相机位置和方向
uniform mat4 projection;  // 投影矩阵：3D -> 2D投影

void main()
{
    // 计算最终顶点位置：projection * view * model * 顶点位置
    // 这个顺序很重要！先应用模型变换，再应用视图变换，最后应用投影
    gl_Position = projection * view * model * vec4(position, 1.0);

    // 将颜色传递到片段着色器
    fragColor = color;
}
