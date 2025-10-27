#version 330 core

// 顶点着色器 - 支持纹理

// 输入：顶点属性
layout(location = 0) in vec3 position;    // 顶点位置
layout(location = 1) in vec3 color;       // 顶点颜色（可选）
layout(location = 2) in vec2 texCoord;    // 纹理坐标

// 输出：传递到片段着色器
out vec3 fragColor;
out vec2 fragTexCoord;

// Uniform变量
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // 计算顶点位置
    gl_Position = projection * view * model * vec4(position, 1.0);

    // 传递数据到片段着色器
    fragColor = color;
    fragTexCoord = texCoord;
}
